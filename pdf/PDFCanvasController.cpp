#include "PDFCanvasController.h"

#include <QtCore/QRectF>

#include <KoZoomController.h>
#include <KoZoomHandler.h>

#include "PDFDocument.h"
#include "PDFCanvas.h"
#include "PDFSelection.h"

class PDFCanvasController::Private
{
public:
    Private(PDFCanvasController *qq) : q(qq) { }
    
    void updatePanGesture(const QPointF &location);
    void updateHandles();
    void moveDocumentOffset(const QPoint &offset);

    PDFCanvasController *q;
    PDFDocument *document;
    PDFCanvas *canvas;

    QPoint oldOffset;
};

PDFCanvasController::PDFCanvasController ( QDeclarativeItem* parent )
    : CMCanvasControllerDeclarative ( parent ), d(new Private(this))
{
    CMProcessInputInterface::setupConnections(inputProxy(), this);
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

void PDFCanvasController::loadDocument()
{
    emit progress(1);
    d->document = new PDFDocument(this, QUrl(file()));
    d->document->open();

    d->canvas = new PDFCanvas(d->document, this);
    setCanvas(d->canvas);
    //connect(d->document, SIGNAL(newPage(int)), d->canvas, SLOT(update()));
    connect(proxyObject, SIGNAL(moveDocumentOffset(QPoint)), d->canvas, SLOT(setDocumentOffset(QPoint)));
    connect(d->document, SIGNAL(documentSizeChanged(QSizeF)), zoomController(), SLOT(setDocumentSize(QSizeF)));

    emit progress(100);
    emit completed();
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
}


#include "PDFCanvasController.moc"
