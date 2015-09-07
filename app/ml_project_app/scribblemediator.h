#ifndef SCRIBBLEMEDIATOR_H
#define SCRIBBLEMEDIATOR_H

#include <QObject>
#include <QVector>
#include <QPointF>
#include <QColor>


class QGraphicsPathItem;
class QGraphicsView;
class QMouseEvent;

// A mediator between a QGraphicsView, where the user draws scribbles, and whoever wishes
// to receive those scribbles. Each time the user draws a new scribble, this class adds the
// scribble path to the QGraphicsView so that it is drawn, and fires the 'scribbleAdded' signal.
class ScribbleMediator : public QObject
{
    Q_OBJECT
public:
    explicit ScribbleMediator(QGraphicsView *parent = 0);

    void setEnabled(bool); // enable or disable scribble drawing
    void setColor(QColor color); // set the color of the next drawn scribble.

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
