#include "CMTablesCanvas.h"

#include <QtGui/QGraphicsWidget>
#include <QtCore/QTimer>

#include <tables/part/Doc.h>
#include <tables/part/CanvasItem.h>
#include <tables/Map.h>
#include <KoView.h>
#include <KoZoomController.h>
#include <KoDocumentInfo.h>

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

    QString file;
    bool hasNextSheet;
    bool hasPreviousSheet;

    void updateCanvas();
};

CMTablesCanvas::CMTablesCanvas(QDeclarativeItem* parent)
    : CMCanvasControllerDeclarative(parent), d(new Private(this))
{

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
    d->updateCanvas();
}

QString CMTablesCanvas::file() const
{
    return d->file;
}

void CMTablesCanvas::setFile(const QString& file)
{
    Calligra::Tables::Doc* doc = new Calligra::Tables::Doc();
    d->file = file;
    d->doc = doc;
    d->updateCanvas();

    QTimer::singleShot(10, this, SLOT(openFile()));
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
    d->updateCanvas();
}

void CMTablesCanvas::updateDocumentSizePrivate(const QSize& size)
{
    zoomController()->setDocumentSize(size);
}

void CMTablesCanvas::openFile()
{
    if(!d->doc->openUrl(KUrl(d->file))) {
        kWarning() << "Could not open file:" << d->file;
        return;
    }

    d->updateCanvas();
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


