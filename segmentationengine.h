#ifndef SEGMENTATIONENGINE_H
#define SEGMENTATIONENGINE_H

#include "scribblemaskgenerator.h"
#include <QPainterPath>
#include <QList>
#include <QMap>

class SegmentationEngine
{
public:
    void reset(QImage image);
    void addScribble(QPainterPath path, int labelId);
    void recompute();

    QList<int> getLabels();
    QBitmap getMaskOf(int labelId);

private:
    QImage _image;
    QMap<int, QList<QPainterPath>> _scribbles;
};

#endif // SEGMENTATIONENGINE_H
