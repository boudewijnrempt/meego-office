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
#include <KDebug>

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

    void beginPinch();
    void updatePinch(qreal scale);
    void endPinch();

    CMCanvasControllerDeclarative* canvasController;
    QGraphicsPixmapItem* scaleProxy;

    bool updateCanvas;

    QPointF centerPoint;
    qreal touchPinchScale;
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

void CMCanvasInputProxy::handleTouchBegin(QTouchEvent* event)
{
    if(event->touchPoints().count() > 1) {
        event->accept();
        d->touchPinchScale = 1.0;
        d->beginPinch();
    }
}

void CMCanvasInputProxy::handleTouchEnd(QTouchEvent* event)
{
    if(event->touchPoints().count() > 1) {
        event->accept();
        d->endPinch();
    }
}

void CMCanvasInputProxy::handleTouchUpdate(QTouchEvent* event)
{
    event->accept();

    QList<QTouchEvent::TouchPoint> touchPoints = event->touchPoints();
    if (touchPoints.count() == 2) {
        // determine scale factor
        const QTouchEvent::TouchPoint &touchPoint0 = touchPoints.first();
        const QTouchEvent::TouchPoint &touchPoint1 = touchPoints.last();
        qreal currentScaleFactor =
                QLineF(touchPoint0.pos(), touchPoint1.pos()).length()
                / QLineF(touchPoint0.startPos(), touchPoint1.startPos()).length();
        
        d->touchPinchScale *= currentScaleFactor;

        d->centerPoint = (touchPoint0.pos() - touchPoint1.pos() * 0.5) + touchPoint1.pos();
    }
    
    d->updatePinch(d->touchPinchScale);
}

void CMCanvasInputProxy::Private::handlePinchGesture(QPinchGesture* pinch)
{
    if(!pinch)
        return;
    
    switch(pinch->state())
    {
        case Qt::GestureStarted:
        {
            beginPinch();
            break;
        }
        case Qt::GestureUpdated:
        {
            centerPoint = pinch->centerPoint();
            updatePinch(pinch->totalScaleFactor());
            break;
        }
        case Qt::GestureFinished:
        {
            endPinch();
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

    qDebug() << "swipe-swipe";
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

void CMCanvasInputProxy::Private::beginPinch()
{
    updateCanvas = false;
    
    qreal zoomX, zoomY;
    canvasController->zoomHandler()->zoom(&zoomX, &zoomY);

    canvasController->setZoomMax(1.0 + (KoZoomMode::maximumZoom() - zoomX));
    canvasController->setZoomMin(KoZoomMode::minimumZoom() / zoomX);

    QPointF origin = canvasController->mapToScene(canvasController->x(), canvasController->y());
    scaleProxy->setPixmap(QPixmap::grabWindow(QApplication::activeWindow()->winId(),
                                              origin.x(), origin.y(),
                                              canvasController->width(), canvasController->height()));
    scaleProxy->setScale(1.0);
    scaleProxy->setVisible(true);
    canvasController->canvas()->canvasItem()->setVisible(false);
}

void CMCanvasInputProxy::Private::updatePinch(qreal scale)
{
    scaleProxy->setTransformOriginPoint(centerPoint);
    //qreal newScale = scale;
    if(scale > scaleProxy->scale() && scale > canvasController->zoomMax()) {
        return;
    } else if(scale < scaleProxy->scale() && scale < canvasController->zoomMin()) {
        return;
    }
    scaleProxy->setScale(scale);
}

void CMCanvasInputProxy::Private::endPinch()
{
    scaleProxy->setVisible(false);
    canvasController->canvas()->canvasItem()->setVisible(true);
    canvasController->zoomBy(centerPoint.toPoint(), scaleProxy->scale());
    updateCanvas = true;
}
