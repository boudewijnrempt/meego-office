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
        ,dragging(false)
        , zoom(1.0)
        , currentGesture(NoGesture)
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

    QString file;

    CMCanvasInputProxy* inputProxy;

    CMCanvasControllerDeclarative* q;
    KoCanvasBase* canvas;
    KoZoomHandler* zoomHandler;
    KoZoomController* zoomController;

    QTimer* timer;
    QTimer tapAndHoldTimer;

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

    enum { NoGesture, PanGesture, TapAndHoldGesture, UpdateClipboardAndClearSelection } currentGesture;
    QPointF currentMousePos;
    struct {
        QTextCursor textCursor;
        QPointF cursorPos, anchorPos;
        KoShape *shape;
    } selection;
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

    d->tapAndHoldTimer.setInterval(QApplication::startDragTime());
    d->tapAndHoldTimer.setSingleShot(true);
    connect(&d->tapAndHoldTimer, SIGNAL(timeout()), this, SLOT(onTapAndHoldGesture()));

    d->mass = 10.f;
    d->dragCoeff = 0.05f;
    d->springCoeff = 0.8f;
    d->timeStep = 1000.f / d->timer->interval();

    connect(this, SIGNAL(heightChanged()), this, SLOT(onHeightChanged()));
    connect(this, SIGNAL(widthChanged()), this, SLOT(onWidthChanged()));

    connect(proxyObject, SIGNAL(moveDocumentOffset(QPoint)), this, SLOT(documentOffsetMoved(QPoint)));

    connect(d->inputProxy, SIGNAL(nextPage()), SIGNAL(nextPage()));
    connect(d->inputProxy, SIGNAL(previousPage()), SIGNAL(previousPage()));
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
}

void CMCanvasControllerDeclarative::resetDocumentOffset(const QPoint& offset)
{
    QPoint o = offset;
    setDocumentOffset(o);
    proxyObject->emitMoveDocumentOffset(offset);
    if(d->inputProxy->updateCanvas()) {
        d->updateCanvasSize();
    }
    d->updateSelectionMarkerPositions();
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

void CMCanvasControllerDeclarative::ensureVisible(KoShape* shape)
{
    Q_UNUSED(shape)
}

void CMCanvasControllerDeclarative::ensureVisible(const QRectF& rect, bool smooth)
{
    Q_UNUSED(rect)
    Q_UNUSED(smooth)
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

static QRectF selectionBoundingBox(QTextCursor &cursor)
{
    QTextFrame *frame = cursor.document()->rootFrame();

    QRectF retval(-5E6,0,105E6,1);
    if (cursor.position() == -1)
        return retval;

    QTextFrame::iterator it;
    for (it = frame->begin(); !(it.atEnd()); ++it) {
        QTextBlock block = it.currentBlock();

        if (cursor.selectionStart() >= block.position()
                && cursor.selectionStart() < block.position() + block.length()) {
            QTextLine line = block.layout()->lineForTextPosition(cursor.selectionStart() - block.position());
            if (line.isValid()) {
                retval.setTop(line.y());
                retval.setLeft(line.cursorToX(cursor.selectionStart() - block.position()));
            }
        }
        if (cursor.selectionEnd() >= block.position()
                && cursor.selectionEnd() < block.position() + block.length()) {
            QTextLine line = block.layout()->lineForTextPosition(cursor.selectionEnd() - block.position());
            if (line.isValid()) {
                retval.setBottom(line.y() + line.height());
                retval.setRight(line.cursorToX(cursor.selectionEnd() - block.position()));
            }
        }
    }
    return retval;
}

void CMCanvasControllerDeclarative::Private::updateSelection(int option)
{
    KWCanvasBase *kwcanvasitem = dynamic_cast<KWCanvasBase *>(q->canvas()->canvasItem());
    KWViewMode *mode = kwcanvasitem ? kwcanvasitem->viewMode() : 0;

    QPointF canvasMousePos = currentMousePos + q->documentOffset();
    QPointF docMousePos = mode ? mode->viewToDocument(canvasMousePos) : q->canvas()->viewConverter()->viewToDocument(canvasMousePos);
    KoShape *shapeUnderCursor = q->canvas()->shapeManager()->shapeAt(docMousePos);
    if (!shapeUnderCursor) {
        if (option == UpdateClipboardAndClearSelection)
            clearSelection();
        return;
    }
    KoTextShapeData *shapeData = qobject_cast<KoTextShapeData *>(shapeUnderCursor->userData());
    if (!shapeData)
        return;

    q->canvas()->shapeManager()->selection()->select(shapeUnderCursor);
    KoToolManager::instance()->switchToolRequested("TextToolFactory_ID");

    QTextDocument *doc = shapeData->document();
    KoTextDocumentLayout *lay = qobject_cast<KoTextDocumentLayout *>(doc->documentLayout());
    KoTextEditor *editor = KoTextDocument(doc).textEditor();
    QPointF shapeMousePos = shapeUnderCursor->absoluteTransformation(0).inverted().map(docMousePos);
    QPointF textDocMousePos = shapeMousePos + QPointF(0.0, shapeData->documentOffset());

    int cursorPos = lay->hitTest(textDocMousePos, Qt::FuzzyHit);
    if (option == ProcessTextUnderMouse) {
        editor->setPosition(cursorPos);
        if (!editor->charFormat().anchorHref().isEmpty()) { // user clicked on link
            emit q->linkActivated(editor->charFormat().anchorHref());
            return;
        }
        editor->select(QTextCursor::WordUnderCursor);
    } else if (option == MovePosition) {
        editor->setPosition(cursorPos, QTextCursor::KeepAnchor);
    } else if (option == MoveAnchor) {
        int oldPosition = editor->position();
        editor->setPosition(cursorPos);
        editor->setPosition(oldPosition, QTextCursor::KeepAnchor);
    } else if (option == UpdateClipboardAndClearSelection) {
        QTextCursor cursor(*editor->cursor());
        if ((cursor.position() <= cursorPos && cursor.anchor() >= cursorPos)
                || (cursor.anchor() <= cursorPos && cursor.position() >= cursorPos)) { // user clicked on selection
            QMimeData *mimeData = new QMimeData;
            QTextDocumentFragment fragment(cursor);
            mimeData->setText(fragment.toPlainText());
            mimeData->setHtml(fragment.toHtml("utf-8"));
            QBuffer buffer;
            QTextDocumentWriter writer(&buffer, "ODF");
            writer.write(fragment);
            buffer.close();
            mimeData->setData("application/vnd.oasis.opendocument.text", buffer.data());
            QApplication::clipboard()->setMimeData(mimeData);
            emit q->textCopiedToClipboard();
        }
        clearSelection();
        return;
    }

    q->canvas()->updateCanvas(shapeUnderCursor->boundingRect());
    selection.textCursor = *editor->cursor();
    updateSelectionMarkerPositions();
}

void CMCanvasControllerDeclarative::Private::updateSelectionMarkerPositions()
{
    if (selection.textCursor.isNull())
        return;
    KWCanvasBase *kwcanvasitem = dynamic_cast<KWCanvasBase *>(q->canvas()->canvasItem());
    KWViewMode *mode = kwcanvasitem ? kwcanvasitem->viewMode() : 0;
    QTextDocument *doc = selection.textCursor.document();
    KoTextDocumentLayout *lay = qobject_cast<KoTextDocumentLayout *>(doc->documentLayout());

    KoShape *shape1 = lay->shapeForPosition(selection.textCursor.position());
    if(!shape1 || shape1->shapeId() != TextShape_SHAPEID) {
        return;
    }
    
    KoTextShapeData *shapeData1 = qobject_cast<KoTextShapeData *>(shape1->userData());
    QTextCursor c1(selection.textCursor);
    c1.clearSelection();
    QPointF positionBottomRight = shape1->absoluteTransformation(0).map(selectionBoundingBox(c1).bottomRight() - QPointF(0, shapeData1->documentOffset()));
    selection.cursorPos = (mode ? mode->documentToView(positionBottomRight) : q->canvas()->viewConverter()->documentToView(positionBottomRight)) - q->documentOffset();

    KoShape *shape2 = lay->shapeForPosition(selection.textCursor.anchor());
    KoTextShapeData *shapeData2 = qobject_cast<KoTextShapeData *>(shape2->userData());
    QTextCursor c2(selection.textCursor);
    c2.setPosition(selection.textCursor.anchor());
    QPointF anchorBottomRight = shape2->absoluteTransformation(0).map(selectionBoundingBox(c2).bottomRight() - QPointF(0, shapeData2->documentOffset()));

    selection.anchorPos = (mode ? mode->documentToView(anchorBottomRight) : q->canvas()->viewConverter()->documentToView(anchorBottomRight)) - q->documentOffset();

    emit q->cursorPosChanged();
    emit q->anchorPosChanged();
}

void CMCanvasControllerDeclarative::moveMarker(int which, qreal x, qreal y)
{
    d->currentMousePos = QPointF(x, y);
    d->updateSelection(which == 1 ? Private::MovePosition : Private::MoveAnchor);
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

QPointF CMCanvasControllerDeclarative::cursorPos() const
{
    return d->selection.cursorPos;
}

QPointF CMCanvasControllerDeclarative::anchorPos() const
{
    return d->selection.anchorPos;
}

void CMCanvasControllerDeclarative::onTapAndHoldGesture()
{
    d->currentGesture = Private::TapAndHoldGesture;
    d->updateSelection(Private::ProcessTextUnderMouse);
}

bool CMCanvasControllerDeclarative::eventFilter(QObject* target , QEvent* event )
{
    if(target == this || target == d->canvas->canvasItem()) {
        if(event->type() == QEvent::GraphicsSceneMousePress) {
            QGraphicsSceneMouseEvent *me = static_cast<QGraphicsSceneMouseEvent *>(event);
            d->velocity = QVector2D();
            d->timer->stop();
            d->currentGesture = Private::NoGesture;
            d->tapAndHoldTimer.start();
            d->currentMousePos = me->pos();
            return true;
        } else if(event->type() == QEvent::GraphicsSceneMouseMove) {
            QGraphicsSceneMouseEvent *me = static_cast<QGraphicsSceneMouseEvent *>(event);
            d->currentMousePos = me->pos();
            if (d->currentGesture == Private::NoGesture
                    && (me->pos() - me->buttonDownPos(Qt::LeftButton)).manhattanLength() >= QApplication::startDragDistance()) {
                d->currentGesture = Private::PanGesture;
                d->tapAndHoldTimer.stop();
            } else if (d->currentGesture == Private::TapAndHoldGesture) {
                d->updateSelection(Private::MovePosition);
            }

            if (d->currentGesture == Private::PanGesture) {
                if(d->inputProxy->updateCanvas())
                    d->inputProxy->handleMouseMoveEvent(static_cast<QGraphicsSceneMouseEvent*>(event));
            }

            return true;
        } else if(event->type() == QEvent::GraphicsSceneMouseRelease) {
            d->timer->start();
            d->tapAndHoldTimer.stop();
            if (d->currentGesture == Private::NoGesture)
                d->updateSelection(Private::UpdateClipboardAndClearSelection);
            return true;
        } else if(event->type() == QEvent::GraphicsSceneMouseDoubleClick) {
            event->accept();
            return true;
        } else if(event->type() == QEvent::TouchBegin) {
            event->accept();
            return true;
        } else if(event->type() == QEvent::Gesture) {
            d->inputProxy->handleGesture(static_cast<QGestureEvent*>(event));
            return true;
        }
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

void CMCanvasControllerDeclarative::setMargin(int margin)
{
    KoCanvasController::setMargin(margin);
    d->updateMinMax();
}

void CMCanvasControllerDeclarative::onHeightChanged()
{
    d->updateCanvasSize();
}

void CMCanvasControllerDeclarative::onWidthChanged()
{
    d->updateCanvasSize();
}

void CMCanvasControllerDeclarative::Private::updateMinMax()
{
    minX = -(q->width() * vastScrollingFactor) - q->margin();
    minY = -(q->height() * vastScrollingFactor) - q->margin();
    maxX = (q->documentSize().width() + (q->width() * vastScrollingFactor) ) - q->width() + q->margin();
    maxY = (q->documentSize().height() + (q->height() * vastScrollingFactor) ) - q->height() + q->margin();
}

void CMCanvasControllerDeclarative::Private::updateCanvasSize()
{
    if(canvas) {
        canvas->canvasItem()->setX(0);
        canvas->canvasItem()->setY(0);
        canvas->canvasItem()->setGeometry(0, 0, q->width(), q->height());
        canvas->updateCanvas(canvas->viewConverter()->viewToDocument(QRectF(0, 0, q->width(), q->height())));
        canvas->canvasItem()->update();
    }
}

void CMCanvasControllerDeclarative::documentOffsetMoved(const QPoint& point)
{
    Q_UNUSED(point);
    d->updateMinMax();
    d->updateSelectionMarkerPositions();
}

void CMCanvasControllerDeclarative::timerUpdate()
{
    QVector2D position = QVector2D(documentOffset());
    QVector2D totalForce = d->force - (d->dragCoeff * d->velocity);
    d->force = QVector2D();

    QVector2D accel = totalForce / d->mass;

    position += d->velocity;

    bool positionValid = true;
    if(position.x() < d->minX) {
        position.setX(position.x() * d->springCoeff);
        positionValid = false;
    }
    if(position.y() < d->minY) {
        position.setY(position.y() * d->springCoeff);
        positionValid = false;
    }

    if(position.x() > d->maxX) {
        float diff = position.x() - d->maxX;
        position.setX(d->maxX + diff * 0.9);
        positionValid = false;
    }
    if(position.y() > d->maxY) {
        float diff = position.y() - d->maxY;
        position.setY(d->maxY + diff * 0.9);
        positionValid = false;
    }

    d->velocity += accel * d->timeStep;

    resetDocumentOffset(QPoint(position.x(), position.y()));


    if(d->velocity.x() > -0.1f && d->velocity.x() < 0.1f && d->velocity.y() > -0.1f && d->velocity.y() < 0.1f && positionValid)
        d->timer->stop();
}

QVector2D CMCanvasControllerDeclarative::force() const
{
    return d->force;
}

void CMCanvasControllerDeclarative::setForce(const QVector2D& newForce)
{
    d->force = newForce;
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

