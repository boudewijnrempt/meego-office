#include "PDFCanvasController.h"

#include <KoZoomController.h>
#include <KoZoomHandler.h>

#include "PDFDocument.h"
#include "PDFCanvas.h"

class PDFCanvasController::Private
{
public:
    PDFDocument *document;
    PDFCanvas *canvas;
};

PDFCanvasController::PDFCanvasController ( QDeclarativeItem* parent )
    : CMCanvasControllerDeclarative ( parent ), d(new Private)
{

}

PDFCanvasController::~PDFCanvasController()
{

}

void PDFCanvasController::loadDocument()
{
    d->document = new PDFDocument(this, QUrl(file()));
    d->document->open();

    d->canvas = new PDFCanvas(d->document, this);
    setCanvas(d->canvas);
    connect(d->document, SIGNAL(newPage(int)), d->canvas, SLOT(update()));
    connect(proxyObject, SIGNAL(moveDocumentOffset(QPoint)), d->canvas, SLOT(setDocumentOffset(QPoint)));
    connect(d->document, SIGNAL(documentSizeChanged(QSizeF)), zoomController(), SLOT(setDocumentSize(QSizeF)));

    d->document->requestPage(0);
}
