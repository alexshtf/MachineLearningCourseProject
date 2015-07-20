#include "segmentationengine.h"
#include "scribblemaskgenerator.h"

void SegmentationEngine::reset(QImage image)
{
    _image = image;
}

void SegmentationEngine::addScribble(QPainterPath path, int labelId)
{
    _scribbles[labelId].append(path);
}

void SegmentationEngine::recompute()
{
    // TODO: Here we will implement our segmentation
}

QList<int> SegmentationEngine::getLabels()
{
    return _scribbles.keys();
}

QBitmap SegmentationEngine::getMaskOf(int labelId)
{
    // TODO: Currently, the mask is simply the scribble itself. We don't yet
    // compute any actual segmentation.

    ScribbleMaskGenerator generator;
    generator.setSize(_image.size());
    for(const auto& path : _scribbles[labelId])
        generator.addScribble(path);

    return generator.getMask();
}
