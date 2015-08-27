#ifndef SEGMENTATIONENGINE_H
#define SEGMENTATIONENGINE_H

#include "scribblemaskgenerator.h"
#include "PixelsLabelsArray.h"
#include <QPainterPath>
#include <QList>
#include <QMap>
#include <boost/multi_array.hpp>

class SegmentationEngine
{
public:
    void reset(QImage image);
    void addScribble(QPainterPath path, int labelId);
    void recompute();

    QList<int> getLabels();
    QBitmap getMaskOf(int labelId);

    void saveSimilarity(const QString& fileName);

private:
    Common::PixelsLabelsArray computeSimilarity();
    class GridMRF makeMrf(Common::PixelsLabelsArray similarity);

    QImage _image;
    Common::PixelsLabelsArray _descriptors;
    QMap<int, QList<QPainterPath>> _scribbles;
    QMap<int, ScribbleMaskGenerator> _generators;
    boost::multi_array<size_t, 2> _segmentation;
};

#endif // SEGMENTATIONENGINE_H
