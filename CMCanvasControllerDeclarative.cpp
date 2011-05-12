#include "CMCanvasControllerDeclarative.h"

#include <QtCore/QTimer>
#include <QtGui/QGesture>
#include <QtGui/QVector2D>
#include <QtGui/QGraphicsWidget>
#include <QtGui/QGraphicsSceneMouseEvent>
#include <QtGui/QApplication>
#include <QtCore/QBuffer>
#include <QtGui/QTextDocumentWriter>
#include <QtGui/QTextDocumentFragment>
#include <QtGui/QTextCursor>

#include <KDE/KActionCollection>

#include <words/part/KWCanvasBase.h>
#include <words/part/KWViewMode.h>

#include <KoShape.h>
#include <KoSelection.h>
#include <KoToolProxy.h>
#include <KoTextDocumentLayout.h>
#include <KoTextEditor.h>
#include <KoTextShapeData.h>
#include <KoToolSelection.h>
#include <KoTextLayoutRootArea.h>
#include <KoCanvasBase.h>
#include <KoViewConverter.h>
#include <KoToolManager.h>
#include <KoZoomHandler.h>
#include <KoZoomController.h>
#include <KoShapeManager.h>
#include <qgesture.h>
#include <QPainter>
#include <QGraphicsScene>
#include <QStyleOptionGraphicsItem>
#include <KoPACanvasItem.h>
#include <KoPACanvasBase.h>
#include <QVector2D>
#include "CMCanvasInputProxy.h"

#define TextShape_SHAPEID "TextShapeID"

class CMCanvasControllerDeclarative::Private
{
public:
    Private(CMCanvasControllerDeclarative* qq)
        : inputProxy( new CMCanvasInputProxy(qq))
        , q(qq)
        , canvas(0)
        , zoomHandler(0)
        , zoomController(0)
        , vastScrollingFactor(0.f)
        , minX(0)
        , minY(0)
        , maxX(0)
        , maxY(0)
        , dragging(false)
        , zoom(1.0)
        , moveThreshold(5)
        , verticalScrollHandle(0)
        , horizontalScrollHandle(0)
    {
        selection.cursorPos = selection.anchorPos = QPointF(-1000, -1000);
    }
    ~Private() { }

    void updateMinMax();
    void updateCanvasSize();
    void checkBounce(const QPoint& offset);
    void frameChanged(int frame);

    enum { ProcessTextUnderMouse, MovePosition, MoveAnchor };
    void updateSelection(int option);
    void clearSelection();
    void updateSelectionMarkerPositions();

    void updateScrollHandles();

    void onHeightChanged();
    void onWidthChanged();
    void documentOffsetMoved(const QPoint& point);
    void timerUpdate();
    void onTapAndHoldGesture();
    
    QString file;

    CMCanvasInputProxy* inputProxy;

    CMCanvasControllerDeclarative* q;
    KoCanvasBase* canvas;
    KoZoomHandler* zoomHandler;
    KoZoomController* zoomController;

    QTimer* timer;

    qreal vastScrollingFactor;

    qreal minX;
    qreal minY;
    qreal maxX;
    qreal maxY;

    bool dragging;
    qreal distX;
    qreal distY;

    qreal zoom;
    qreal zoomMax;
    qreal zoomMin;

    QVector2D force;
    QVector2D velocity;

    float mass;
    float dragCoeff;
    float timeStep;
    float springCoeff;
    qreal moveThreshold;

    QPointF currentMousePos;
    struct {
        QTextCursor textCursor;
        QPointF cursorPos, anchorPos;
        KoShape *shape;
    } selection;

    QDeclarativeItem *verticalScrollHandle;
    QDeclarativeItem *horizontalScrollHandle;
};

CMCanvasControllerDeclarative::CMCanvasControllerDeclarative(QDeclarativeItem* parent)
    : QDeclarativeItem(parent), KoCanvasController(0), d(new Private(this))
{
    setClip(true);
    setAcceptedMouseButtons(Qt::LeftButton);
    setFiltersChildEvents(true);
    setAcceptTouchEvents(true);
    grabGesture(Qt::PinchGesture);
    grabGesture(Qt::SwipeGesture);
    
    d->timer = new QTimer(this);
    d->timer->setInterval(40);
    connect(d->timer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
    
    d->mass = 10.f;
    d->dragCoeff = 0.01f;
    d->springCoeff = 0.6f;
    d->timeStep = 1000.f / d->timer->interval();

    connect(this, SIGNAL(heightChanged()), this, SLOT(updateCanvasSize()));
    connect(this, SIGNAL(widthChanged()), this, SLOT(updateCanvasSize()));

    connect(proxyObject, SIGNAL(moveDocumentOffset(QPoint)), this, SLOT(documentOffsetMoved(QPoint)));

    connect(d->inputProxy, SIGNAL(nextPage()), SIGNAL(nextPage()));
    connect(d->inputProxy, SIGNAL(previousPage()), SIGNAL(previousPage()));
    connect(d->inputProxy, SIGNAL(endPanGesture()), d->timer, SLOT(start()));
}

CMCanvasControllerDeclarative::~CMCanvasControllerDeclarative()
{
    KoToolManager::instance()->removeCanvasController(this);
}

QString CMCanvasControllerDeclarative::file() const
{
    return d->file;
}

void CMCanvasControllerDeclarative::setFile(const QString &f)
{
    d->file = f;
}

void CMCanvasControllerDeclarative::setVastScrolling(qreal factor)
{
    d->vastScrollingFactor = factor;
    d->updateMinMax();
}

void CMCanvasControllerDeclarative::setZoomWithWheel(bool zoom)
{
    Q_UNUSED(zoom)
}

void CMCanvasControllerDeclarative::updateDocumentSize(const QSize& sz, bool recalculateCenter)
{
    Q_UNUSED(recalculateCenter)
    proxyObject->emitSizeChanged(sz);
    KoCanvasController::setDocumentSize(sz);
    d->updateMinMax();
    d->updateScrollHandles();
    
    emit documentSizeChanged();
}

void CMCanvasControllerDeclarative::resetDocumentOffset(const QPoint& offset)
{
    QPoint o;
    if(!offset.isNull()) {
        o = offset;
    } else {
        o = QPoint(d->minX, d->minY);
    }
    setDocumentOffset(o);
    proxyObject->emitMoveDocumentOffset(offset);
    if(d->inputProxy->updateCanvas()) {
        d->updateCanvasSize();
    }
    d->updateSelectionMarkerPositions();
    d->updateScrollHandles();
}

void CMCanvasControllerDeclarative::setScrollBarValue(const QPoint& value)
{
    Q_UNUSED(value)
}

QPoint CMCanvasControllerDeclarative::scrollBarValue() const
{
    return documentOffset();
}

void CMCanvasControllerDeclarative::pan(const QPoint& distance)
{
    Q_UNUSED(distance)
}

QPoint CMCanvasControllerDeclarative::preferredCenter() const
{
    return QPoint();
}

void CMCanvasControllerDeclarative::setPreferredCenter(const QPoint& viewPoint)
{
    Q_UNUSED(viewPoint)
}

void CMCanvasControllerDeclarative::recenterPreferred()
{
}

void CMCanvasControllerDeclarative::zoomTo(const QRect& rect)
{
    Q_UNUSED(rect)
}

void CMCanvasControllerDeclarative::zoomBy(const QPoint& center, qreal zoom)
{
    qreal tempZoom = d->zoom * zoom;

    if(tempZoom > KoZoomMode::minimumZoom() && tempZoom < KoZoomMode::maximumZoom()) {
        proxyObject->emitZoomBy(zoom);
        d->zoom = tempZoom;

        QPointF offset = documentOffset();
        QPointF position;
        position.rx() = (zoom * -offset.x()) + (1 - zoom) * center.x();
        position.ry() = (zoom * -offset.y()) + (1 - zoom) * center.y();

        QPoint oNew = (-position).toPoint();

        d->updateMinMax();
        d->updateCanvasSize();
        resetDocumentOffset(oNew);
        d->timer->start();
    }
}

void CMCanvasControllerDeclarative::zoomOut(const QPoint& center)
{
    if(center.isNull()) {
        zoomBy(QPoint(width()/ 2, height() / 2), sqrt(0.5));
    } else {
        zoomBy(center, sqrt(0.5));
    }
}

void CMCanvasControllerDeclarative::zoomIn(const QPoint& center)
{
    if(center.isNull()) {
        zoomBy(QPoint(width()/ 2, height() / 2), sqrt(2.0));
    } else {
        zoomBy(center, sqrt(2.0));
    }
}

void CMCanvasControllerDeclarative::resetZoom()
{
    d->zoomController->setZoom(KoZoomMode::ZOOM_CONSTANT, 1.0);
    d->updateMinMax();
    d->updateCanvasSize();
    d->updateSelectionMarkerPositions();
}

void CMCanvasControllerDeclarative::tapTriggered(QPoint pos)
{
    qDebug() << "Triggered tap at position" << pos;
    //handleShortTap(pos);
}

void CMCanvasControllerDeclarative::ensureVisible(KoShape* shape)
{
    Q_UNUSED(shape)
}

void CMCanvasControllerDeclarative::ensureVisible(const QRectF& rect, bool smooth)
{
    QPointF target = rect.center();
    target.rx() -= width() / 2;
    target.ry() -= height() / 2;

    if(-target.x() < d->minX) {
        target.setX(-d->minX);
    }
    if(-target.y() < d->minY) {
        target.setY(-d->minY);
    }
    if(-target.x() > d->maxX) {
        target.setX(-d->maxX);
    }
    if(-target.y() > d->maxY) {
        target.setY(-d->maxY);
    }

    if(!smooth) {
        resetDocumentOffset(target.toPoint());
    } else {
        QVector2D diff = QVector2D(target) - QVector2D(documentOffset());
        d->force += diff / 10;
        d->timer->start();
    }
}

int CMCanvasControllerDeclarative::canvasOffsetY() const
{
    return documentOffset().y();
}

int CMCanvasControllerDeclarative::canvasOffsetX() const
{
    return documentOffset().x();
}

int CMCanvasControllerDeclarative::visibleWidth() const
{
    return width();
}

int CMCanvasControllerDeclarative::visibleHeight() const
{
    return height();
}

KoCanvasBase* CMCanvasControllerDeclarative::canvas() const
{
    return d->canvas;
}

void CMCanvasControllerDeclarative::setCanvas(KoCanvasBase* canvas)
{
    Q_ASSERT(canvas);

    if (d->canvas) {
        proxyObject->emitCanvasRemoved(this);
        d->canvas->canvasItem()->hide();
        canvas->setCanvasController(0);
        canvas->canvasItem()->removeSceneEventFilter(this);
    }

    d->canvas = canvas;

    KoToolManager::instance()->addController(this);

    zoomController(canvas->viewConverter(), true);

    canvas->setCanvasController(this);
    canvas->canvasItem()->setParentItem(this);
    canvas->canvasItem()->installEventFilter(this);
    canvas->canvasItem()->setAcceptTouchEvents(true);
    canvas->canvasItem()->grabGesture(Qt::PinchGesture);
    canvas->canvasItem()->grabGesture(Qt::SwipeGesture);
    canvas->canvasItem()->show();

    d->updateCanvasSize();
    d->updateMinMax();

    resetDocumentOffset(QPoint(d->minX, d->minY));
    setFocusProxy(d->canvas->canvasItem());
    proxyObject->emitCanvasSet(this);
    d->timer->start();
}

void CMCanvasControllerDeclarative::setDrawShadow(bool drawShadow)
{
    Q_UNUSED(drawShadow)
}

QSize CMCanvasControllerDeclarative::viewportSize() const
{
    return QSize(width(), height());
}

void CMCanvasControllerDeclarative::scrollContentsBy(int dx, int dy)
{
    if(!d->inputProxy->updateCanvas())
        return;

    if(d->minX == d->maxX || d->minY == d->maxY)
        d->updateMinMax();


    QPoint offset = documentOffset();
    offset.setX(offset.x() + dx);
    offset.setY(offset.y() + dy);

    resetDocumentOffset(offset);
}

QDeclarativeItem* CMCanvasControllerDeclarative::horizontalScrollHandle()
{
    return d->horizontalScrollHandle;
}

QDeclarativeItem* CMCanvasControllerDeclarative::verticalScrollHandle()
{
    return d->verticalScrollHandle;
}

void CMCanvasControllerDeclarative::setHorizontalScrollHandle ( QDeclarativeItem* handle )
{
    d->horizontalScrollHandle = handle;
}

void CMCanvasControllerDeclarative::setVerticalScrollHandle ( QDeclarativeItem* handle )
{
    d->verticalScrollHandle = handle;
}

QVector2D CMCanvasControllerDeclarative::force() const
{
    return d->force;
}

void CMCanvasControllerDeclarative::setForce(const QVector2D& newForce)
{
    d->force = newForce;
    emit docMoved();
}

void CMCanvasControllerDeclarative::setZoomMax(qreal newZoomMax)
{
    d->zoomMax = newZoomMax;
}

qreal CMCanvasControllerDeclarative::zoomMax() const
{
    return d->zoomMax;
}

void CMCanvasControllerDeclarative::setZoomMin(qreal newZoomMin)
{
    d->zoomMin = newZoomMin;
}

qreal CMCanvasControllerDeclarative::zoomMin() const
{
    return d->zoomMin;
}

KoZoomHandler* CMCanvasControllerDeclarative::zoomHandler() const
{
    return d->zoomHandler;
}

bool CMCanvasControllerDeclarative::eventFilter(QObject* target , QEvent* event )
{
    if(target == this || target == d->canvas->canvasItem()) {
        return d->inputProxy->handleEvent(event);
    }
    return QDeclarativeItem::eventFilter(target, event);
}

KoZoomController* CMCanvasControllerDeclarative::zoomController(KoViewConverter* viewConverter, bool recreate)
{
    if(recreate) {
        delete d->zoomController;
        d->zoomController = 0;
    }

    if(!d->zoomController) {
        d->zoomHandler = dynamic_cast<KoZoomHandler*>(viewConverter);
        if(!d->zoomHandler)
            d->zoomHandler = new KoZoomHandler();

        KActionCollection collection(this);
        d->zoomController = new KoZoomController(this, d->zoomHandler, &collection, 0, this);
    }
    return d->zoomController;
}

QPointF CMCanvasControllerDeclarative::cursorPos() const
{
    return d->selection.cursorPos;
}

QPointF CMCanvasControllerDeclarative::anchorPos() const
{
    return d->selection.anchorPos;
}

void CMCanvasControllerDeclarative::setMargin(int margin)
{
    KoCanvasController::setMargin(margin);
    d->updateMinMax();
}

void CMCanvasControllerDeclarative::moveMarker(int which, qreal x, qreal y)
{
    d->currentMousePos = QPointF(x, y);
    d->updateSelection(which == 1 ? Private::MovePosition : Private::MoveAnchor);
}

void CMCanvasControllerDeclarative::Private::updateSelection(int option)
{
//     KWCanvasBase *kwcanvasitem = dynamic_cast<KWCanvasBase *>(q->canvas()->canvasItem());
//     KWViewMode *mode = kwcanvasitem ? kwcanvasitem->viewMode() : 0;
// 
//     QPointF canvasMousePos = currentMousePos + q->documentOffset();
//     QPointF docMousePos = q->canvas()->viewConverter()->viewToDocument(canvasMousePos);
//     KoShape *shapeUnderCursor = q->canvas()->shapeManager()->shapeAt(docMousePos);
//     if (!shapeUnderCursor) {
//         if (option == UpdateClipboardAndClearSelection)
//             clearSelection();
//         return;
//     }
//     KoTextShapeData *shapeData = qobject_cast<KoTextShapeData *>(shapeUnderCursor->userData());
//     if (!shapeData)
//         return;
// 
//     q->canvas()->shapeManager()->selection()->select(shapeUnderCursor);
//     KoToolManager::instance()->switchToolRequested("TextToolFactory_ID");
// 
//     KoTextLayoutRootArea *root = shapeData->rootArea();
//     QTextDocument *doc = shapeData->document();
//     KoTextEditor *editor = KoTextDocument(doc).textEditor();
//     QPointF shapeMousePos = shapeUnderCursor->absoluteTransformation(0).inverted().map(docMousePos);
//     QPointF textDocMousePos = shapeMousePos + QPointF(0.0, shapeData->documentOffset());
// 
//     int cursorPos = root->hitTest(textDocMousePos, Qt::FuzzyHit);
//     if (option == ProcessTextUnderMouse) {
//         editor->setPosition(cursorPos);
//         if (!editor->charFormat().anchorHref().isEmpty()) { // user clicked on link
//             emit q->linkActivated(editor->charFormat().anchorHref());
//             return;
//         }
//         editor->select(QTextCursor::WordUnderCursor);
//     } else if (option == MovePosition) {
//         editor->setPosition(cursorPos, QTextCursor::KeepAnchor);
//     } else if (option == MoveAnchor) {
//         int oldPosition = editor->position();
//         editor->setPosition(cursorPos);
//         editor->setPosition(oldPosition, QTextCursor::KeepAnchor);
//     } else if (option == UpdateClipboardAndClearSelection) {
//         QTextCursor cursor(*editor->cursor());
//         if ((cursor.position() <= cursorPos && cursor.anchor() >= cursorPos)
//                 || (cursor.anchor() <= cursorPos && cursor.position() >= cursorPos)) { // user clicked on selection
//             QMimeData *mimeData = new QMimeData;
//             QTextDocumentFragment fragment(cursor);
//             mimeData->setText(fragment.toPlainText());
//             mimeData->setHtml(fragment.toHtml("utf-8"));
//             QBuffer buffer;
//             QTextDocumentWriter writer(&buffer, "ODF");
//             writer.write(fragment);
//             buffer.close();
//             mimeData->setData("application/vnd.oasis.opendocument.text", buffer.data());
//             QApplication::clipboard()->setMimeData(mimeData);
//             emit q->textCopiedToClipboard();
//         }
//         clearSelection();
//         return;
//     }

//     q->canvas()->updateCanvas(shapeUnderCursor->boundingRect());
//     selection.textCursor = *editor->cursor();
//     updateSelectionMarkerPositions();
}

void CMCanvasControllerDeclarative::Private::updateSelectionMarkerPositions()
{
//     if (selection.textCursor.isNull())
//         return;
//     KWCanvasBase *kwcanvasitem = dynamic_cast<KWCanvasBase *>(q->canvas()->canvasItem());
//     KWViewMode *mode = kwcanvasitem ? kwcanvasitem->viewMode() : 0;
//     QTextDocument *doc = selection.textCursor.document();
// 
//     QTextCursor cursor = selection.textCursor;
//     QTextLine line = cursor.block().layout()->lineForTextPosition(cursor.positionInBlock());
//     if(line.isValid()) {
//         QRectF textRect(line.cursorToX(cursor.positionInBlock()) , line.y(), 1, line.height());
//         selection.cursorPos = textRect.center() - q->documentOffset();
//     }
// 
//     cursor = QTextCursor(doc);
//     cursor.setPosition(selection.textCursor.anchor());
//     line = cursor.block().layout()->lineForTextPosition(cursor.positionInBlock());
//     if(line.isValid()) {
//         QRectF textRect = QRectF(line.cursorToX(cursor.positionInBlock()) , line.y(), 1, line.height());
//         selection.anchorPos = textRect.center() - q->documentOffset();
//     }
// 
//     emit q->cursorPosChanged();
//     emit q->anchorPosChanged();
}

void CMCanvasControllerDeclarative::Private::clearSelection()
{
    if (selection.textCursor.isNull())
        return;
    QTextDocument *doc = selection.textCursor.document();
    KoTextEditor *editor = KoTextDocument(doc).textEditor();
    editor->clearSelection();

    selection.textCursor = QTextCursor();
    selection.cursorPos = selection.anchorPos = QPointF(-1000, -1000);
    emit q->cursorPosChanged();
    emit q->anchorPosChanged();
}

CMCanvasInputProxy* CMCanvasControllerDeclarative::inputProxy()
{
    return d->inputProxy;
}

void CMCanvasControllerDeclarative::Private::onTapAndHoldGesture()
{
    updateSelection(Private::ProcessTextUnderMouse);
}

void CMCanvasControllerDeclarative::Private::updateMinMax()
{
    if(q->canvasMode() != KoCanvasController::Infinite ) {
        int halfWindowWidth = q->width() / 2;
        int halfDocWidth = q->documentSize().width() / 2;
        int margin = q->margin();

        minX = halfWindowWidth - (halfDocWidth + margin) + qMin(0, halfWindowWidth - halfDocWidth);
        maxX = minX + ( qMax(0, -(halfWindowWidth - halfDocWidth)) + q->margin()) * 2;

        int halfWindowHeight = q->height() / 2;
        int halfDocHeight = q->documentSize().height() / 2;

        minY = halfWindowHeight - (halfDocHeight + margin) + qMin(0, halfWindowHeight - halfDocHeight);
        maxY = minY + ( qMax(0, -(halfWindowHeight - halfDocHeight)) + q->margin()) * 2;
    } else {
        minX = 1 - q->documentSize().width();
        maxX = 1;
        minY = 1 - q->documentSize().height();
        maxY = 1;
    }
    q->resetDocumentOffset(QPoint(minX, minY));
    updateSelectionMarkerPositions();
}

void CMCanvasControllerDeclarative::Private::updateCanvasSize()
{
    if(canvas) {
        canvas->canvasItem()->setX(0);
        canvas->canvasItem()->setY(0);
        canvas->canvasItem()->setGeometry(0, 0, q->width(), q->height());
        canvas->updateCanvas(canvas->viewConverter()->viewToDocument(QRectF(0, 0, q->width(), q->height())));
        canvas->canvasItem()->update();

        updateSelectionMarkerPositions();
    }
}

void CMCanvasControllerDeclarative::Private::documentOffsetMoved(const QPoint& point)
{
    Q_UNUSED(point);
    //d->updateMinMax();
    updateSelectionMarkerPositions();
}

void CMCanvasControllerDeclarative::Private::timerUpdate()
{
    QVector2D position = QVector2D(q->documentOffset());
    QVector2D totalForce = force - (dragCoeff * velocity);
    force = QVector2D();

    QVector2D accel = totalForce / mass;

    position += velocity;

    bool positionValid = true;
    if(-position.x() < minX) {
        float diff = (position.x() + minX) * springCoeff;
        position.setX(-minX + diff);
        if(qAbs(diff) > moveThreshold) {
            positionValid = false;
        } else {
            position.setX(-minX);
        }
    }
    if(-position.y() < minY) {
        float diff = (position.y() + minY) * springCoeff;
        position.setY(-minY + diff * springCoeff);
        if(qAbs(diff) > moveThreshold) {
            positionValid = false;
        } else {
            position.setY(-minY);
        }
    }

    if(-position.x() > maxX) {
        float diff = (position.x() + maxX) * springCoeff;
        position.setX(-maxX + diff);
        if(qAbs(diff) > moveThreshold) {
            positionValid = false;
        } else {
            position.setX(-maxX);
        }
    }

    if(-position.y() > maxY) {
        float diff = (position.y() + maxY) * springCoeff;
        position.setY(-maxY + diff);
        if(qAbs(diff) > moveThreshold) {
            positionValid = false;
        } else {
            position.setY(-maxY);
        }
    }

    velocity += accel * timeStep;

    q->resetDocumentOffset(QPoint(position.x(), position.y()));

    if(qAbs(velocity.x()) < moveThreshold && qAbs(velocity.y()) < moveThreshold && positionValid) {
        timer->stop();
        emit q->hideHorizontalScrollHandle();
        emit q->hideVerticalScrollHandle();
    }
}

void CMCanvasControllerDeclarative::Private::updateScrollHandles()
{
    if(horizontalScrollHandle) {
        qreal docWidthPerc = q->width() / q->documentSize().width();
        if(docWidthPerc < 1.0) {
            horizontalScrollHandle->setWidth(q->width() * docWidthPerc);
            emit q->showHorizontalScrollHandle();
        } else {
            horizontalScrollHandle->setWidth(q->width());
            emit q->hideHorizontalScrollHandle();
        }

        qreal offset = qreal(q->documentOffset().x()) / q->documentSize().width();
        qreal pos = offset * q->width();
        horizontalScrollHandle->setX(pos);
    }

    if(verticalScrollHandle) {
        qreal docHeightPerc = q->height() / q->documentSize().height();
        if(docHeightPerc < 1.0) {
            verticalScrollHandle->setHeight(q->height() * docHeightPerc);
            emit q->showVerticalScrollHandle();
        } else {
            verticalScrollHandle->setHeight(q->height());
            emit q->hideVerticalScrollHandle();
        }

        qreal offset = qreal(q->documentOffset().y()) / q->documentSize().height();
        qreal pos = offset * q->height();
        verticalScrollHandle->setY(pos);
    }
}

#include "CMCanvasControllerDeclarative.moc"
