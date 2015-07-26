#ifndef SCRIBBLEMASKGENERATOR_H
#define SCRIBBLEMASKGENERATOR_H

#include <QSize>
#include <QBitmap>

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
