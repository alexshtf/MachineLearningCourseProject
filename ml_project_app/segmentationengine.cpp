#include "segmentationengine.h"

#include "scribblemaskgenerator.h"
#include "PixelsLabelsArray.BIF.h"
#include "computesegmentationworker.h"
#include "config.h"

#include <QPainter>
#include <QtDebug>

#include <algorithm>
#include <cassert>
#include <cmath>

namespace {

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

Common::PixelsLabelsArray computeDescriptors(const QImage& image, size_t neighborhoodSize)
{
    auto descriptorDim = (2 * neighborhoodSize + 1) * (2 * neighborhoodSize + 1) * 3; // number of pixels x 3 channels
    Common::PixelsLabelsArray result(image.height(), image.width(), descriptorDim);

    auto szx = static_cast<int>(neighborhoodSize);
    auto szy = static_cast<int>(neighborhoodSize);

    for(int x = 0; x < image.width(); ++x)
    {
        for(int y = 0; y < image.height(); ++y)
        {
            size_t l = 0;
            for(int dx = -szx; dx <= szx; ++dx)
            {
                for(int dy = -szy; dy <= szy; ++dy)
                {
                    auto rgb = rgbAt(image, x + dx, y + dy);
                    auto hsv = rgb.toHsv();
                    result.At(y, x, l + 0) = hsv.hueF();
                    result.At(y, x, l + 1) = hsv.saturationF();
                    result.At(y, x, l + 2) = hsv.valueF();

                    l += 3;
                    assert(l <= descriptorDim);
                }
            }
        }
    }

    return result;
}

}

SegmentationEngine::SegmentationEngine(const Config &config, ComputeSegmentationWorker *worker, QObject *parent)
    : QObject(parent)
    , _config(config)
    , _worker(worker)
{
    connect(this, &SegmentationEngine::compute, _worker, &ComputeSegmentationWorker::compute, Qt::QueuedConnection);

    connect(_worker, &ComputeSegmentationWorker::startedRecompute, this, &SegmentationEngine::startedRecompute, Qt::QueuedConnection);
    connect(_worker, &ComputeSegmentationWorker::trainedSVM, this, &SegmentationEngine::trainedSVM, Qt::QueuedConnection);
    connect(_worker, &ComputeSegmentationWorker::computedSimilarity, this, &SegmentationEngine::computedSimilarity, Qt::QueuedConnection);
    connect(_worker, &ComputeSegmentationWorker::createdMRF, this, &SegmentationEngine::createdMRF, Qt::QueuedConnection);
    connect(_worker, &ComputeSegmentationWorker::mapInitialized, this, &SegmentationEngine::mapInitialized, Qt::QueuedConnection);
    connect(_worker, &ComputeSegmentationWorker::iterationFinished, this, &SegmentationEngine::iterationFinished, Qt::QueuedConnection);
    connect(_worker, &ComputeSegmentationWorker::recomputeDone, this, &SegmentationEngine::onRecomputeDone, Qt::QueuedConnection);
}

SegmentationEngine::~SegmentationEngine()
{
}

void SegmentationEngine::reset(QImage image)
{
    _image = image;
    _scribbles.clear();
    _generators.clear();
    _descriptors = computeDescriptors(_image, _config.getNeighborhoodSize());
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
    emit compute(_image, _descriptors, getLabelPixels(_generators));
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
    // TODO: Find a solution t similarity saving
    //Common::SaveBIF(computeSimilarity(), fileName.toStdString());
}

void SegmentationEngine::onRecomputeDone(boost::multi_array<size_t, 2> segmentation)
{
    _segmentation.resize(boost::extents[_image.height()][_image.width()]);
    _segmentation = segmentation;

    emit recomputeDone();
}
