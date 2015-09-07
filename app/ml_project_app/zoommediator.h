#ifndef ZOOMPANMEDIATOR_H
#define ZOOMPANMEDIATOR_H

#include <QObject>
#include <QGraphicsView>
#include <QWheelEvent>

class ZoomMediator : public QObject
{
    Q_OBJECT
public:
    explicit ZoomMediator(QGraphicsView *parent = 0);
    void setEnabled(bool flag);

private:
    void GraphicsViewWheelEvent(QWheelEvent* wheelEvent);
    bool eventFilter(QObject *, QEvent *) override;

    QGraphicsView* _parent;
    bool _isEnabled;

};

#endif // ZOOMPANMEDIATOR_H
