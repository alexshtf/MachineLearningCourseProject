#include "zoommediator.h"

namespace {
const double SENSITIVITY = 0.001;
}

ZoomMediator::ZoomMediator(QGraphicsView *parent) : QObject(parent),
    _parent(parent),
    _isEnabled(false)
{
    parent->viewport()->installEventFilter(this);
}

void ZoomMediator::setEnabled(bool isEnabled)
{
    _isEnabled = isEnabled;
}

bool ZoomMediator::eventFilter(QObject* object, QEvent* event)
{
    if (object == _parent->viewport() && event->type() == QEvent::Wheel)
    {
        auto wheelEvent = reinterpret_cast<QWheelEvent*>(event);
        GraphicsViewWheelEvent(wheelEvent);
        return true;
    }

    return QObject::eventFilter(object, event);
}

void ZoomMediator::GraphicsViewWheelEvent(QWheelEvent* wheelEvent)
{
    wheelEvent->accept();

    if (!_isEnabled)
        return;

    auto delta = wheelEvent->angleDelta().y();
    auto scaleFactor = std::exp(SENSITIVITY * delta);
    _parent->scale(scaleFactor, scaleFactor);
}

