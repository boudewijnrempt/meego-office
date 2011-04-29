#include "CMTablesCanvas.h"

#include <QtGui/QGraphicsWidget>
#include <QtCore/QTimer>

#include <tables/part/Doc.h>
#include <tables/part/CanvasItem.h>
#include <tables/Map.h>
#include <KoView.h>
#include <KoZoomController.h>
#include <KoDocumentInfo.h>

#include "CMProgressProxy.h"
#include <Sheet.h>

class CMTablesCanvas::Private
{
public:
    Private(CMTablesCanvas* qq)
        : q(qq),
          activeSheetIndex(0),
          doc(0),
          canvas(0)
    {}

    CMTablesCanvas* q;

    int activeSheetIndex;
    Calligra::Tables::Doc* doc;
    Calligra::Tables::CanvasItem* canvas;

    bool hasNextSheet;
    bool hasPreviousSheet;

    void updateCanvas();
};

CMTablesCanvas::CMTablesCanvas(QDeclarativeItem* parent)
    : CMCanvasControllerDeclarative(parent), d(new Private(this))
{
    connect(this, SIGNAL(nextPage()), SLOT(nextSheet()));
    connect(this, SIGNAL(previousPage()), SLOT(previousSheet()));
}

CMTablesCanvas::~CMTablesCanvas()
{
    delete d;
}

int CMTablesCanvas::activeSheetIndex() const
{
    return d->activeSheetIndex;
}

QObject* CMTablesCanvas::doc() const
{
    return d->doc;
}

void CMTablesCanvas::setActiveSheetIndex(int index)
{
    d->activeSheetIndex = index;
    emit sheetChanged(d->activeSheetIndex);
    d->updateCanvas();
}

bool CMTablesCanvas::hasNextSheet() const
{
    return d->hasNextSheet;
}

void CMTablesCanvas::nextSheet()
{
    d->activeSheetIndex++;
    if(d->activeSheetIndex >= d->doc->map()->count()) {
        d->activeSheetIndex = 0;
    }
    emit sheetChanged(d->activeSheetIndex);
    d->updateCanvas();
}

bool CMTablesCanvas::hasPreviousSheet() const
{
    return d->hasPreviousSheet;
}

void CMTablesCanvas::previousSheet()
{
    d->activeSheetIndex--;
    if(d->activeSheetIndex < 0) {
        d->activeSheetIndex = d->doc->map()->count() - 1;
    }
    emit sheetChanged(d->activeSheetIndex);
    d->updateCanvas();
}

void CMTablesCanvas::changeSheet(int newIndex)
{
    d->canvas->setActiveSheet( d->doc->map()->sheet(newIndex) );
    emit sheetChanged(newIndex);
}

void CMTablesCanvas::updateDocumentSizePrivate(const QSize& size)
{
    zoomController()->setDocumentSize(size);
}

QString CMTablesCanvas::sheetName() const
{
    return d->canvas->activeSheet()-> sheetName();
}

void CMTablesCanvas::loadDocument()
{
    emit progress(1);

    setCanvasMode(KoCanvasController::Infinite);

    Calligra::Tables::Doc* doc = new Calligra::Tables::Doc();
    d->doc = doc;
    d->updateCanvas();

    CMProgressProxy *proxy = new CMProgressProxy(this);
    doc->setProgressProxy(proxy);

    connect(proxy, SIGNAL(valueChanged(int)), SIGNAL(progress(int)));

    if(!d->doc->openUrl(KUrl(file()))) {
        kWarning() << "Could not open file:" << file();
        return;
    }

    d->updateCanvas();

    emit progress(100);
    emit completed();
    emit sheetChanged(0);
}

void CMTablesCanvas::Private::updateCanvas()
{
    if (canvas && canvas->doc() != doc) {
        delete canvas;
        canvas = 0;
    }

    if (!canvas && doc != 0) {

        canvas = static_cast<Calligra::Tables::CanvasItem*>(doc->canvasItem());
        q->setCanvas(canvas);

        connect(q->proxyObject, SIGNAL(moveDocumentOffset(const QPoint&)), canvas, SLOT(setDocumentOffset(QPoint)));
        connect(canvas, SIGNAL(documentSizeChanged(QSize)), q, SLOT(updateDocumentSizePrivate(QSize)));
    }

    canvas->updateCanvas(QRectF(0, 0, q->width(), q->height()));
    if (canvas && activeSheetIndex >= 0) {
        canvas->setActiveSheet(doc->map()->sheet(activeSheetIndex));
    }
}


void CMTablesCanvas::handleShortTap(QPointF pos)
{
    // select the shape under the current position and then activate the text tool, send mouse events
}
