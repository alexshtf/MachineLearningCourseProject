#include "segmentationengine.h"
#include "starupdatemrfmap.h"
#include "neighborhoods.h"
#include "config.h"
#include "svm.h"

#include <QPoint>
#include <QVector>
#include <QColor>
#include <QImage>

namespace {

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

double dist(const QImage& image, const Pixel& left, const Pixel& right)
{
    auto c1 = QColor::fromRgb(image.pixel(left.col(), left.row()));
    auto c2 = QColor::fromRgb(image.pixel(right.col(), right.row()));

    auto rDiff = c1.redF() - c2.redF();
    auto gDiff = c1.greenF() - c2.greenF();
    auto bDiff = c1.blueF() - c2.blueF();

    return std::sqrt(rDiff*rDiff + gDiff*gDiff + bDiff*bDiff);
}

} // anonymous namespace

SegmentationEngine::SegmentationEngine(const Config &config, QObject *parent)
    : QObject(parent)
    , _config(config)
{
}

void SegmentationEngine::compute(const QImage &image, const Common::PixelsLabelsArray &descriptors, QMap<int, QVector<QPoint> > scribbles)
{
    emit startedRecompute();

    auto similarity = computeSimilarity(descriptors, scribbles);
    emit computedSimilarity();

    auto mrf = makeMrf(std::move(similarity), image);
    emit createdMRF();

    StarUpdateMRFMap mrfMap(mrf);
    mrfMap.init();
    emit mapInitialized();

    uint num = 0;
    auto prevDualEnergy = std::numeric_limits<double>::min();
    auto currDualEnergy = mrfMap.computeDualEnergy();
    auto epsilon = _config.getStoppingEpsilon() * (mrf.rows() * mrf.cols()); // epsilon is normalized by image size
    while (std::abs(prevDualEnergy - currDualEnergy) > epsilon)
    {
        mrfMap.nextIteration();
        prevDualEnergy = currDualEnergy;
        currDualEnergy = mrfMap.computeDualEnergy();

        emit iterationFinished(num++, currDualEnergy);
    }

    mrfMap.computePrimal();
    emit recomputeDone(mrfMap.primal());
}

Common::PixelsLabelsArray SegmentationEngine::computeSimilarity(const Common::PixelsLabelsArray &descriptors, QMap<int, QVector<QPoint>> scribbles)
{
    auto similarity = Common::PixelsLabelsArray(descriptors.Rows(), descriptors.Cols(), scribbles.size());
    if (similarity.Labels() <= 1) // we need at-least two labels to perform segmentation
    {
        std::fill(similarity.Data(), similarity.Data() + similarity.Size(), 0.0);
        return similarity;
    }

    for(auto label : scribbles.keys())
    {
        auto param = makeSvmParameter(scribbles.size());
        auto prob = makeSvmProblem(descriptors, scribbles, label);
        if (!svm_check_parameter(&prob, &param))
        {
            auto model = svm_train(&prob, &param);
            emit trainedSVM();

            fillSimilarity(similarity, descriptors, model, label);
            svm_free_and_destroy_model(&model);
        }
    }

    return similarity;
}

GridMRF SegmentationEngine::makeMrf(Common::PixelsLabelsArray similarity, const QImage &image)
{
    GridMRF mrf(similarity.Rows(), similarity.Cols(), similarity.Labels(), 6);

    // negate similarity (our MRF inference engine finds argmax and not argmin)
    mrf.setUnary(std::move(similarity));

    // set pairwise potentials of 4-neighborhood (negative, again, since we look for argmax)
    for(size_t r = 0; r < mrf.rows(); ++r)
    {
        for(size_t c = 0; c < mrf.cols(); ++c)
        {
            Pixel p(r, c);
            for(const auto& n : FourNeighbors(mrf.rows(), mrf.cols(), p))
                mrf.setPairwise(p, n, -_config.getLambda() * dist(image, p, n));
        }
    }

    return mrf;
}
