#include "scribblemediator.h"
#include <QGraphicsView>
#include <QEvent>
#include <QMouseEvent>
#include <QGraphicsPathItem>
#include <QDebug>

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
        default:
            break;
        }
    }

    return QObject::eventFilter(object, event);
}

void ScribbleMediator::startScribble(QMouseEvent* event)
{
    if (!_enabled)
        return;

    auto scenePoint = _parent->mapToScene(event->pos());
    QPainterPath path(scenePoint);

    QPen pen(_color);
    pen.setCosmetic(true);
    pen.setWidth(3);

    _pathItem = new QGraphicsPathItem;
    _pathItem->setPath(path);
    _pathItem->setPen(pen);

    _parent->scene()->addItem(_pathItem);
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
