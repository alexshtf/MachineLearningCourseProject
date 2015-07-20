#ifndef SCRIBBLEMEDIATOR_H
#define SCRIBBLEMEDIATOR_H

#include <QObject>
#include <QVector>
#include <QPointF>
#include <QColor>

class QGraphicsPathItem;
class QGraphicsView;
class QMouseEvent;
class ScribbleMediator : public QObject
{
    Q_OBJECT
public:
    explicit ScribbleMediator(QGraphicsView *parent = 0);

    void setEnabled(bool);
    void setColor(QColor color);

signals:
    void scribbleAdded(QGraphicsPathItem* item);

private:
    bool eventFilter(QObject *, QEvent *) override;
    void startScribble(QMouseEvent*);
    void endScribble(QMouseEvent*);
    void scribble(QMouseEvent*);
    void addPathPoint(QMouseEvent* event);
    void resetState();

    QGraphicsView* _parent;
    bool _enabled;
    QColor _color;

    QGraphicsPathItem* _pathItem;
};

#endif // SCRIBBLEMEDIATOR_H
