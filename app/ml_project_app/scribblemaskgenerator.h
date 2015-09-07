#ifndef SCRIBBLEMASKGENERATOR_H
#define SCRIBBLEMASKGENERATOR_H

#include <QSize>
#include <QBitmap>

// Computes the set of pixels that a set of scribbles cover
// Usage:
//   Initialization: Call to setSize()
//   Add scribbles:
//        One or more call to addScribble. Each scribble will be analyzed for the pixels it covers.
//        Pixels outside the boundaries specified by setSize() are ignored.
//   Get pixels:
//        getMask() gets a bitmap where each covered pixel is '1' and others are zeros. The size of the mask
//        is the size specified by setSize()
//        getPixels() gets the list of pixel coordinates that would be '1' if we called getMask()
//   Clear:
//        Clear all scribbles, but do not modify image size.
class ScribbleMaskGenerator
{
public:
    ScribbleMaskGenerator();
    void setSize(QSize size);
    void clear();
    void addScribble(const QPainterPath& path);
    QBitmap getMask() const;
    QVector<QPoint> getPixels() const;

private:
    QBitmap _bitmap;
};

#endif // SCRIBBLEMASKGENERATOR_H
