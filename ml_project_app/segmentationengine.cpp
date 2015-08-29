#include "segmentationengine.h"
#include "scribblemaskgenerator.h"
#include "svm.h"
#include "PixelsLabelsArray.BIF.h"
#include "mrfmap.h"
#include "gridmrf.h"
#include "neighborhoods.h"
#include <QPainter>
#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>

namespace {

const size_t DESCRIPTOR_DIM = 25 * 3; // RGB color of a 5x5 neighborhood

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
        y = 2 * image.height() - y - 1;

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
            for(int dx = -2; dx <= 2; ++dx)
            {
                for(int dy = -2; dy <= 2; ++dy)
                {
                    auto rgb = rgbAt(image, x + dx, y + dy);
                    auto hsv = rgb.toHsv();
                    result.At(y, x, l + 0) = hsv.hueF();
                    result.At(y, x, l + 1) = hsv.saturationF();
                    result.At(y, x, l + 2) = hsv.valueF();

                    l += 3;
                    assert(l <= DESCRIPTOR_DIM);
                }
            }
        }
    }

    return result;
}

svm_parameter makeSvmParameter(size_t labelsCount)
{
    svm_parameter param = {};

    param.svm_type = C_SVC;
    param.kernel_type = RBF;
    param.gamma = 1.0 / labelsCount;
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

void fillSimilarity(Common::PixelsLabelsArray& similarity, const Common::PixelsLabelsArray& descriptors, svm_model* model, int label)
{
    auto val = std::make_unique<svm_node[]>(descriptors.Labels() + 1);
    auto prob = std::make_unique<double[]>(2);

    int svmLabels[2];
    svm_get_labels(model, svmLabels);
    size_t idx = svmLabels[0] == 1 ? 0 : 1;

    for(size_t c = 0; c < similarity.Cols(); ++c)
    {
        for(size_t r = 0; r < similarity.Rows(); ++r)
        {
            fillSvmNodes(val.get(), descriptors, r, c);
            svm_predict_probability(model, val.get(), prob.get());
            similarity.At(r, c, label) = prob[idx];
        }
    }
}

double getMaxValue(const Common::PixelsLabelsArray& segmentation, int x, int y)
{
    auto* pixelLabels = &segmentation.At(y, x, 0);
    auto max = std::max_element(pixelLabels, pixelLabels + segmentation.Labels());
    return *max;
}

double dist(const Pixel& left, const Pixel& right)
{
    auto dr = left.row() - right.row();
    auto dc = left.col() - right.col();
    return std::sqrt(dr * dr + dc * dc);
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
    auto mrf = makeMrf(computeSimilarity());

    EmptyMRFMap mrfMap(mrf);

    mrfMap.init();
    auto prevDualEnergy = std::numeric_limits<double>::min();
    auto currDualEnergy = 0.0;
    auto epsilon = 1E-5;
    while (std::abs(prevDualEnergy - (currDualEnergy = mrfMap.computeDualEnergy())) < epsilon)
    {
        prevDualEnergy = currDualEnergy;
        mrfMap.nextIteration();
    }

    _segmentation.resize(boost::extents[mrf.rows()][mrf.cols()]);
    _segmentation = mrfMap.primal();
}

QList<int> SegmentationEngine::getLabels()
{
    return _scribbles.keys();
}

QBitmap SegmentationEngine::getMaskOf(int labelId)
{
    QBitmap bitmap(_image.width(), _image.height());
    bitmap.fill();

    QPainter painter(&bitmap);
    for(int x = 0; x < bitmap.width(); ++x)
    {
        for(int y = 0; y < bitmap.height(); ++y)
        {
            if (_segmentation[y][x] == labelId)
                painter.setPen(Qt::black);
            else
                painter.setPen(Qt::white);

            painter.drawPoint(x, y);
        }
    }

    return bitmap;
}

void SegmentationEngine::saveSimilarity(const QString &fileName)
{
    Common::SaveBIF(computeSimilarity(), fileName.toStdString());
}

Common::PixelsLabelsArray SegmentationEngine::computeSimilarity()
{
    auto similarity = Common::PixelsLabelsArray(_image.height(), _image.width(), _generators.size());
    if (similarity.Labels() <= 1) // we need at-least two labels to perform segmentation
    {
        std::fill(similarity.Data(), similarity.Data() + similarity.Size(), 0.0);
        return similarity;
    }

    QMap<int, QVector<QPoint>> labelPixels = getLabelPixels(_generators);
    for(auto label : labelPixels.keys())
    {
        auto param = makeSvmParameter(labelPixels.size());
        auto prob = makeSvmProblem(_descriptors, labelPixels, label);
        if (!svm_check_parameter(&prob, &param))
        {
            auto model = svm_train(&prob, &param);
            fillSimilarity(similarity, _descriptors, model, label);
            svm_free_and_destroy_model(&model);
        }
    }

    return similarity;
}

GridMRF SegmentationEngine::makeMrf(Common::PixelsLabelsArray similarity)
{
    GridMRF mrf(similarity.Rows(), similarity.Cols(), similarity.Labels(), 6);
    mrf.setUnary(std::move(similarity));

    // set pairwise potentials of 4-neighborhood
    for(size_t r = 0; r < mrf.rows(); ++r)
    {
        for(size_t c = 0; c < mrf.cols(); ++c)
        {
            Pixel p(r, c);
            for(const auto& n : FourNeighbors(mrf.rows(), mrf.cols(), p))
                mrf.setPairwise(p, n, dist(p, n));
        }
    }

    return mrf;
}
