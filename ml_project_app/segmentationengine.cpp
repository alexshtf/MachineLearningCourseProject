#include "segmentationengine.h"
#include "scribblemaskgenerator.h"
#include "svm.h"
#include <QPainter>
#include <cassert>

namespace {

const size_t DESCRIPTOR_DIM = 9 * 3; // RGB color of 9-neighborhood of each pixel

QMap<int, QVector<QPoint>> getLabelPixels(const QMap<int, ScribbleMaskGenerator>& generators)
{
    QMap<int, QVector<QPoint>> result;
    for(auto key : generators.keys())
        result[key] = generators[key].getPixels();

    return result;
}

QColor rgbAt(const QImage& image, int x, int y)
{
    if (x >= image.width())
        x = 2 * image.width() - x - 1;
    if (y >= image.height())
        y = 2 * image.height() - x - 1;

    return QColor::fromRgb(image.pixel(std::abs(x), std::abs(y)));
}

Common::PixelsLabelsArray computeDescriptors(const QImage& image)
{
    Common::PixelsLabelsArray result(image.height(), image.width(), DESCRIPTOR_DIM);
    for(int x = 0; x < image.width(); ++x)
    {
        for(int y = 0; y < image.height(); ++y)
        {
            size_t l = 0;
            for(int dx = 0; dx < 3; ++dx)
            {
                for(int dy = 0; dy < 3; ++dy)
                {
                    auto rgb = rgbAt(image, x, y);
                    result.At(y, x, l + 0) = rgb.redF();
                    result.At(y, x, l + 1) = rgb.greenF();
                    result.At(y, x, l + 2) = rgb.blueF();

                    l += 3;
                    assert(l <= DESCRIPTOR_DIM);
                }
            }
        }
    }

    return result;
}

svm_parameter makeSvmParameter()
{
    svm_parameter param = {};
    param.svm_type = C_SVC;
    param.kernel_type = LINEAR;
    param.C = 1;
    param.cache_size = 10;
    param.eps = 0.001;
    param.probability = 1;

    return param;
}

void fillSvmNodes(svm_node* result, const Common::PixelsLabelsArray& descriptors, size_t r, size_t c)
{
    auto ls = descriptors.Labels();
    for(int l = 0; l < ls; ++l)
    {
        result[l].index = l;
        result[l].value = descriptors.At(r, c, l);
    }
    result[ls].index = -1;
}

svm_node* makeSvmNodes(const Common::PixelsLabelsArray& descriptors, QPoint pixel)
{
    auto result = std::make_unique<svm_node[]>(descriptors.Labels() + 1);
    fillSvmNodes(result.get(), descriptors, pixel.y(), pixel.x());
    return result.release();
}

svm_problem makeSvmProblem(const Common::PixelsLabelsArray& descriptors, QMap<int, QVector<QPoint>> labelPixels, int trueLabel)
{
    svm_problem problem = {};

    for(const auto& points : labelPixels)
        problem.l += points.size();
    problem.y = new double[problem.l];
    problem.x = new svm_node*[problem.l];

    size_t l = 0;
    for(auto label : labelPixels.keys())
    {
        for(auto pixel : labelPixels[label])
        {
            problem.y[l] = (label == trueLabel);
            problem.x[l] = makeSvmNodes(descriptors, pixel);
            ++l;
        }
    }

    return problem;
}

void updateSegmentation(Common::PixelsLabelsArray& segmentation, const Common::PixelsLabelsArray& descriptors, svm_model* model, int label)
{
    auto val = std::make_unique<svm_node[]>(descriptors.Labels() + 1);
    auto prob = std::make_unique<double[]>(2);

    for(size_t c = 0; c < segmentation.Cols(); ++c)
    {
        for(size_t r = 0; r < segmentation.Rows(); ++r)
        {
            fillSvmNodes(val.get(), descriptors, r, c);
            svm_predict_probability(model, val.get(), prob.get());
            segmentation.At(r, c, label) = prob[1];
        }
    }
}

int getMaxLabel(const Common::PixelsLabelsArray& segmentation, int x, int y)
{
    auto* pixelLabels = &segmentation.At(y, x, 0);
    auto max = std::max_element(pixelLabels, pixelLabels + segmentation.Labels());
    return max - pixelLabels;
}

}

void SegmentationEngine::reset(QImage image)
{
    _image = image;
    _scribbles.clear();
    _generators.clear();
    _descriptors = computeDescriptors(_image);
}

void SegmentationEngine::addScribble(QPainterPath path, int labelId)
{
    _scribbles[labelId].append(path);

    auto generator = _generators.find(labelId);
    if (generator == _generators.end())
    {
        generator = _generators.insert(labelId, ScribbleMaskGenerator());
        generator->setSize(_image.size());
    }

    generator->addScribble(path);
}

void SegmentationEngine::recompute()
{
    QMap<int, QVector<QPoint>> labelPixels = getLabelPixels(_generators);
    _segmentation = Common::PixelsLabelsArray(_image.height(), _image.width(), labelPixels.keys().size());
    if (_segmentation.Labels() <= 1) // we need at-least two labels to perform segmentation
    {
        std::fill(_segmentation.Data(), _segmentation.Data() + _segmentation.Size(), 0.0);
        return;
    }

    for(auto label : labelPixels.keys())
    {
        auto param = makeSvmParameter();
        auto prob = makeSvmProblem(_descriptors, labelPixels, label);
        if (!svm_check_parameter(&prob, &param))
        {
            auto model = svm_train(&prob, &param);
            updateSegmentation(_segmentation, _descriptors, model, label);
            svm_free_and_destroy_model(&model);
        }
    }
}

QList<int> SegmentationEngine::getLabels()
{
    return _scribbles.keys();
}

QBitmap SegmentationEngine::getMaskOf(int labelId)
{
    QBitmap bitmap(_image.width(), _image.height());
    bitmap.fill();
    if (_segmentation.Labels() <= 1)
        return bitmap;

    QPainter painter(&bitmap);
    for(int x = 0; x < bitmap.width(); ++x)
    {
        for(int y = 0; y < bitmap.height(); ++y)
        {
            auto maxLabel = getMaxLabel(_segmentation, x, y);
            if (maxLabel == labelId)
                painter.setPen(Qt::black);
            else
                painter.setPen(Qt::white);

            painter.drawPoint(x, y);
        }
    }

    return bitmap;
}
