#ifndef SEGMENTATIONENGINE_H
#define SEGMENTATIONENGINE_H

#include "scribblemaskgenerator.h"
#include "PixelsLabelsArray.h"
#include "metatypes.h"

#include <QPainterPath>
#include <QList>
#include <QMap>
#include <boost/multi_array.hpp>

class SegmentationEngine : public QObject
{
    Q_OBJECT
public:
    SegmentationEngine(const class Config& config, class ComputeSegmentationWorker* worker, QObject* parent = nullptr);
    ~SegmentationEngine();

    void reset(QImage image);
    void addScribble(QPainterPath path, int labelId);
    void recompute();

    QList<int> getLabels();
    QBitmap getMaskOf(int labelId);

    void saveSimilarity(const QString& fileName);

signals:
    // signals for the UI
    void startedRecompute();
    void trainedSVM();
    void computedSimilarity();
    void createdMRF();
    void mapInitialized();
    void iterationFinished(uint num, double dual);
    void recomputeDone();

    // signal to the worker - to start computing
    void compute(const QImage& image, const Common::PixelsLabelsArray& descriptors, QMap<int, QVector<QPoint>> scribbles);

private slots:
    void onRecomputeDone(boost::multi_array<size_t, 2> segmentation);

private:
    const class Config& _config;
    class ComputeSegmentationWorker* _worker;

    QImage _image;
    Common::PixelsLabelsArray _descriptors;
    QMap<int, QList<QPainterPath>> _scribbles;
    QMap<int, ScribbleMaskGenerator> _generators;
    boost::multi_array<size_t, 2> _segmentation;
};

#endif // SEGMENTATIONENGINE_H
