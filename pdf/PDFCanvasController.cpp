#include "PDFCanvasController.h"

#include <QtCore/QRectF>

#include <KoZoomController.h>
#include <KoZoomHandler.h>

#include "PDFDocument.h"
#include "PDFCanvas.h"
#include "PDFSelection.h"
#include "PDFPage.h"

class PDFCanvasController::Private
{
public:
    Private(PDFCanvasController *qq) : q(qq), document(0), canvas(0) { }
    
    void updatePanGesture(const QPointF &location);
    void updateHandles();
    void moveDocumentOffset(const QPoint &offset);
    void documentLoaded();

    PDFCanvasController *q;
    PDFDocument *document;
    PDFCanvas *canvas;
    PDFPage *currentPage;

    QPoint oldOffset;
};

PDFCanvasController::PDFCanvasController ( QDeclarativeItem* parent )
    : CanvasControllerDeclarative ( parent ), d(new Private(this))
{
    ProcessInputInterface::setupConnections(inputProxy(), this);
    connect(inputProxy(), SIGNAL(updatePanGesture(QPointF)), this, SLOT(updatePanGesture(QPointF)));
    connect(proxyObject, SIGNAL(moveDocumentOffset(QPoint)), this, SLOT(moveDocumentOffset(QPoint)));
}

PDFCanvasController::~PDFCanvasController()
{

}

void PDFCanvasController::copySelection()
{
    d->canvas->selection()->copy();
}

PDFDocument* PDFCanvasController::document() const
{
    return d->document;
}

int PDFCanvasController::pageCount() const
{
    return d->document->pageCount();
}

int PDFCanvasController::page() const
{
    if(d->currentPage) {
        return d->currentPage->pageNumber();
    }

    return -1;
}

void PDFCanvasController::loadDocument()
{
    emit progress(1);
    d->document = new PDFDocument(this, QUrl(file()));
    connect(d->document, SIGNAL(opened()), this, SLOT(documentLoaded()));
    d->document->open();

    d->canvas = new PDFCanvas(d->document, this);
    setCanvas(d->canvas);
    //connect(d->document, SIGNAL(newPage(int)), d->canvas, SLOT(update()));
    connect(proxyObject, SIGNAL(moveDocumentOffset(QPoint)), d->canvas, SLOT(setDocumentOffset(QPoint)));
    connect(d->document, SIGNAL(documentSizeChanged(QSizeF)), zoomController(), SLOT(setDocumentSize(QSizeF)));
}

void PDFCanvasController::setPage ( int newPage )
{
    if(newPage >= 0 && newPage < d->document->pageCount()) {
        d->currentPage = d->document->page(newPage);
        resetDocumentOffset(QPoint(documentOffset().x(), d->currentPage->positionInDocument()));
        emit pageChanged(newPage);
    }
}

void PDFCanvasController::goToNextPage()
{
    if(d->currentPage->pageNumber() + 1 < d->document->pageCount()) {
        setPage(d->currentPage->pageNumber() + 1);
    } else {
        setPage(0);
    }
}

void PDFCanvasController::goToPreviousPage()
{
    if(d->currentPage->pageNumber() - 1 > 0) {
        setPage(d->currentPage->pageNumber() - 1);
    } else {
        setPage(d->document->pageCount() - 1);
    }
}

void PDFCanvasController::Private::updatePanGesture ( const QPointF& location )
{
    PDFSelection *sel = canvas->selection();
    sel->setGeometry(sel->x(), sel->y(), location.x() - sel->x(), location.y() - sel->y());
    canvas->update();
    updateHandles();
}

void PDFCanvasController::onSingleTap ( const QPointF& location )
{
    PDFSelection *sel = d->canvas->selection();
    QPointF selPos(location.x() - sel->x(), location.y() - sel->y());
    if(sel->isVisible()) {
        if(!sel->contains(selPos)) {
            sel->setVisible(false);
            selectionAnchorHandle()->setVisible(false);
            selectionCursorHandle()->setVisible(false);
        } else {
            emit selected( sel->geometry().center() );
        }
    }
}

void PDFCanvasController::onDoubleTap ( const QPointF& location )
{
    Q_UNUSED(location)
}

void PDFCanvasController::onLongTap ( const QPointF& location )
{
    d->canvas->selection()->setVisible(true);
    d->canvas->selection()->setGeometry(location.x(), location.y(), 1, 1);
    d->canvas->selection()->setDocumentOffset(documentOffset());
    d->canvas->selection()->setCurrentPage(d->document->pageAt(location + documentOffset(), d->canvas->scaling()));
    d->canvas->update();
    d->updateHandles();
}

void PDFCanvasController::onLongTapEnd ( const QPointF& location )
{
    emit selected( d->canvas->selection()->geometry().center() );
}

void PDFCanvasController::Private::updateHandles()
{
    PDFSelection *sel = canvas->selection();

    if(!q->selectionAnchorHandle() || !q->selectionCursorHandle() || !sel || !sel->isVisible()) {
        return;
    }
    
    q->selectionAnchorHandle()->setVisible(true);
    q->selectionCursorHandle()->setVisible(true);
    q->selectionAnchorHandle()->setPos(sel->x(), sel->y());
    q->selectionCursorHandle()->setPos(sel->x() + sel->geometry().width(), sel->y() + sel->geometry().height());
}

void PDFCanvasController::Private::moveDocumentOffset( const QPoint &offset )
{
    int dx = offset.x() - oldOffset.x();
    int dy = offset.y() - oldOffset.y();
    canvas->selection()->moveBy(-dx, -dy);
    updateHandles();
    oldOffset = offset;

    currentPage = document->pageAt(QPointF(q->width() / 2, q->height() / 2) + q->documentOffset(), canvas->scaling());
    if(currentPage) {
        emit q->pageChanged(currentPage->pageNumber());
    }
}

void PDFCanvasController::Private::documentLoaded()
{
    q->resetDocumentOffset();
    emit q->progress(100);
    emit q->completed();
}

#include "PDFCanvasController.moc"
