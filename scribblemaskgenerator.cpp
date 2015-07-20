#include "scribblemaskgenerator.h"
#include <QPainter>
#include <QRegion>

ScribbleMaskGenerator::ScribbleMaskGenerator()
{
}

void ScribbleMaskGenerator::setSize(QSize size)
{
    _bitmap = QBitmap(size);
    _bitmap.fill(Qt::white);
}

void ScribbleMaskGenerator::clear()
{
    _bitmap.fill(Qt::white);
}

void ScribbleMaskGenerator::addScribble(const QPainterPath &path)
{
    QPainter painter(&_bitmap);
    painter.setPen(Qt::black);
    painter.drawPath(path);
}

QBitmap ScribbleMaskGenerator::getMask()
{
    return _bitmap;
}

QVector<QPoint> ScribbleMaskGenerator::getPixels()
{
    QVector<QPoint> result;
    for(QRect rect : QRegion(_bitmap).rects())
    {
        for(auto x = rect.left(); x <= rect.right(); ++x)
            for(auto y = rect.top(); y <= rect.bottom(); ++y)
                result.push_back(QPoint(x, y));
    }

    return result;
}

