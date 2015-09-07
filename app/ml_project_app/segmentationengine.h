#ifndef COMPUTESEGMENTATIONWORKER_H
#define COMPUTESEGMENTATIONWORKER_H

#include "PixelsLabelsArray.h"
#include "gridmrf.h"
#include "metatypes.h"
#include <QObject>
#include <QVector>
#include <QMap>
#include <QPoint>

class SegmentationEngine : public QObject
{
    Q_OBJECT
public:
    explicit SegmentationEngine(const class Config& config, QObject *parent = 0);

public slots:
    void compute(const QImage& image, const Common::PixelsLabelsArray& descriptors, QMap<int, QVector<QPoint>> scribbles);

signals:
    void startedRecompute();
    void trainedSVM();
    void computedSimilarity(Common::PixelsLabelsArray similarity);
    void createdMRF();
    void mapInitialized();
    void iterationFinished(uint num, double dual);
    void recomputeDone(boost::multi_array<size_t, 2> segmentation);

private:
    const class Config& _config;

    Common::PixelsLabelsArray computeSimilarity(const Common::PixelsLabelsArray& descriptors, QMap<int, QVector<QPoint>> scribbles);
    GridMRF makeMrf(Common::PixelsLabelsArray similarity, const QImage& image);
};

#endif // COMPUTESEGMENTATIONWORKER_H
