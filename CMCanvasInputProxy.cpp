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
    Private(CMCanvasInputProxy* qq)
        : q(qq), updateCanvas(true)
    { }
    ~Private() {}
    
    CMCanvasInputProxy* q;
    
    void handlePinchGesture(QPinchGesture* pinch);
    void handleSwipeGesture(QSwipeGesture* swipe);

    CMCanvasControllerDeclarative* canvasController;
    QGraphicsPixmapItem* scaleProxy;

    bool updateCanvas;

};

CMCanvasInputProxy::CMCanvasInputProxy(CMCanvasControllerDeclarative* canvas, QObject* parent)
    : QObject(parent)
    , d(new Private(this))
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
    QSwipeGesture* swipe = qobject_cast<QSwipeGesture*>(event->gesture(Qt::SwipeGesture));
    
    if(pinch)
        d->handlePinchGesture(pinch);
    else if(swipe)
        d->handleSwipeGesture(swipe);
}

void CMCanvasInputProxy::Private::handlePinchGesture(QPinchGesture* pinch)
{
    if(!pinch)
        return;
    
    switch(pinch->state())
    {
        case Qt::GestureStarted:
        {
            updateCanvas = false;
            
            qreal zoomX, zoomY;
            canvasController->zoomHandler()->zoom(&zoomX, &zoomY);
            
            canvasController->setZoomMax(1.0 + (KoZoomMode::maximumZoom() - zoomX));
            canvasController->setZoomMin(KoZoomMode::minimumZoom() / zoomX);
            
            scaleProxy->setPixmap(QPixmap::grabWindow(QApplication::activeWindow()->winId(),
                                                      canvasController->x(), canvasController->y(),
                                                      canvasController->width(), canvasController->height()));
            scaleProxy->setScale(1.0);
            scaleProxy->setVisible(true);
            canvasController->canvas()->canvasItem()->setVisible(false);
        }
        case Qt::GestureUpdated:
        {
            scaleProxy->setTransformOriginPoint(pinch->centerPoint());
            qreal newScale = scaleProxy->scale() + (pinch->scaleFactor() - pinch->lastScaleFactor());
            if(newScale > scaleProxy->scale() && newScale > canvasController->zoomMax()) {
                break;
            } else if(newScale < scaleProxy->scale() && newScale < canvasController->zoomMin()) {
                break;
            }
            scaleProxy->setScale(scaleProxy->scale() + (pinch->scaleFactor() - pinch->lastScaleFactor()));
            break;
        }
        case Qt::GestureFinished:
        {
            scaleProxy->setVisible(false);
            canvasController->canvas()->canvasItem()->setVisible(true);
            canvasController->zoomBy(pinch->centerPoint().toPoint(), scaleProxy->scale());
            updateCanvas = true;
            break;
        }
        default:
            // Do nothing
            break;
    }
}

void CMCanvasInputProxy::Private::handleSwipeGesture(QSwipeGesture* swipe)
{
    if(!swipe)
        return;

    switch(swipe->state())
    {
        case Qt::GestureFinished:
        {
            if (swipe->horizontalDirection() == QSwipeGesture::Left
                || swipe->verticalDirection() == QSwipeGesture::Up)
                emit q->previousPage();
            else
                emit q->nextPage();
        }
        default:
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
