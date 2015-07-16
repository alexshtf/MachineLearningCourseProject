#include "scribblemediator.h"
#include <QGraphicsView>
#include <QEvent>
#include <QMouseEvent>
#include <QGraphicsPathItem>

ScribbleMediator::ScribbleMediator(QGraphicsView *parent) : QObject(parent),
    _parent(parent),
    _enabled(false),
    _pathItem(nullptr)
{
    _parent->viewport()->installEventFilter(this);
}

void ScribbleMediator::setEnabled(bool flag)
{
    _enabled = flag;
    resetState();
}

void ScribbleMediator::setColor(QColor color)
{
    _color = color;
    resetState();
}

bool ScribbleMediator::eventFilter(QObject* object, QEvent* event)
{
    if (_enabled && object == _parent->viewport())
    {
        auto mouseEvent = reinterpret_cast<QMouseEvent*>(event);
        switch(event->type())
        {
        case QEvent::MouseButtonPress:
            startScribble(mouseEvent);
            break;
        case QEvent::MouseButtonRelease:
            endScribble(mouseEvent);
            break;
        case QEvent::MouseMove:
            scribble(mouseEvent);
            break;
        }
    }

    return QObject::eventFilter(object, event);
}

void ScribbleMediator::startScribble(QMouseEvent* event)
{
    if (!_enabled)
        return;

    QPainterPath path;

    auto scenePoint = _parent->mapToScene(event->pos());
    path.moveTo(scenePoint);

    _pathItem = _parent->scene()->addPath(path, QPen(_color));
}


void ScribbleMediator::endScribble(QMouseEvent* event)
{
    if (!_enabled)
        return;

    addPathPoint(event);

    emit scribbleAdded(_pathItem);
    _pathItem = nullptr;
}

void ScribbleMediator::scribble(QMouseEvent* event)
{
    if (!_enabled)
        return;

    if (_pathItem)
        addPathPoint(event);
}

void ScribbleMediator::resetState()
{
    if (_pathItem)
    {
        _parent->scene()->removeItem(_pathItem);
        _pathItem = nullptr;
    }
}

void ScribbleMediator::addPathPoint(QMouseEvent* event)
{
    auto path = _pathItem->path();
    path.lineTo(_parent->mapToScene(event->pos()));
    _pathItem->setPath(path);
}
