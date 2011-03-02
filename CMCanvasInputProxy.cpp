#include "CMCanvasInputProxy.h"
#include <QtCore/QPointF>
#include <QtGui/QGraphicsSceneMouseEvent>
#include <QtGui/QTabletEvent>
#include <QtGui/QGesture>
#include <KoZoomMode.h>
#include <QtGui/QPixmap>
#include <QtGui/QApplication>
#include "CMCanvasControllerDeclarative.h"
#include <KoCanvasBase.h>
#include <KoZoomHandler.h>
#include <QtGui/QGraphicsWidget>

class CMCanvasInputProxy::Private
{
public:
    Private()
        : updateCanvas(true)
    { }
    ~Private() {}

    CMCanvasControllerDeclarative* canvasController;
    QGraphicsPixmapItem* scaleProxy;

    bool updateCanvas;

};

CMCanvasInputProxy::CMCanvasInputProxy(CMCanvasControllerDeclarative* canvas)
    : d(new Private())
{
    d->canvasController = canvas;
    
    d->scaleProxy = new QGraphicsPixmapItem(QPixmap(), canvas);
    d->scaleProxy->setVisible(false);
}

CMCanvasInputProxy::~CMCanvasInputProxy()
{

}

CMCanvasControllerDeclarative* CMCanvasInputProxy::canvasController() const
{
    return d->canvasController;
}

void CMCanvasInputProxy::setCanvasController(CMCanvasControllerDeclarative* newController)
{
    d->canvasController = newController;
}

void CMCanvasInputProxy::handleMouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    QPointF prev = event->lastPos();
    QPointF cur = event->pos();
    
    QVector2D force = d->canvasController->force();
    
    force.setX(prev.x() - cur.x());
    force.setY(prev.y() - cur.y());
    
    d->canvasController->setForce(force);
    d->canvasController->scrollContentsBy(force.x(), force.y());
}

void CMCanvasInputProxy::handleGesture(QGestureEvent* event)
{
    QPinchGesture* pinch = qobject_cast<QPinchGesture*>(event->gesture(Qt::PinchGesture));
    
    if(!pinch)
        return;
    
    switch(pinch->state())
    {
        case Qt::GestureStarted:
        {
            d->updateCanvas = false;
            
            qreal zoomX, zoomY;
            d->canvasController->zoomHandler()->zoom(&zoomX, &zoomY);
            
            d->canvasController->setZoomMax(1.0 + (KoZoomMode::maximumZoom() - zoomX));
            d->canvasController->setZoomMin(KoZoomMode::minimumZoom() / zoomX);
            
            d->scaleProxy->setPixmap(QPixmap::grabWindow(QApplication::activeWindow()->winId(), d->canvasController->x(), d->canvasController->y(), d->canvasController->width(), d->canvasController->height()));
            d->scaleProxy->setScale(1.0);
            d->scaleProxy->setVisible(true);
            d->canvasController->canvas()->canvasItem()->setVisible(false);
        }
        case Qt::GestureUpdated:
        {
            d->scaleProxy->setTransformOriginPoint(pinch->centerPoint());
            qreal newScale = d->scaleProxy->scale() + (pinch->scaleFactor() - pinch->lastScaleFactor());
            if(newScale > d->scaleProxy->scale() && newScale > d->canvasController->zoomMax()) {
                break;
            } else if(newScale < d->scaleProxy->scale() && newScale < d->canvasController->zoomMin()) {
                break;
            }
            d->scaleProxy->setScale(d->scaleProxy->scale() + (pinch->scaleFactor() - pinch->lastScaleFactor()));
            break;
        }
        case Qt::GestureFinished:
            d->scaleProxy->setVisible(false);
            d->canvasController->canvas()->canvasItem()->setVisible(true);
            d->canvasController->zoomBy(pinch->centerPoint().toPoint(), d->scaleProxy->scale());
            d->updateCanvas = true;
            break;
    }
}

bool CMCanvasInputProxy::updateCanvas() const
{
    return d->updateCanvas;
}

void CMCanvasInputProxy::setUpdateCanvas(bool newUpdate)
{
    d->updateCanvas = newUpdate;
}
