#include "CMStageCanvas.h"

#include <QtGui/QGraphicsItem>
#include <KDE/KDebug>
#include <KoPACanvasItem.h>
#include <part/KPrDocument.h>
#include <part/KPrView.h>
#include "CMStageDeclarativeView.h"
#include "CMProgressProxy.h"

class CMStageCanvas::Private
{
public:
    Private(CMStageCanvas* qq)
        : q(qq), doc(0), canvas(0)
    { }
    ~Private() { }

    CMStageCanvas* q;

    KPrDocument* doc;
    KoPACanvasItem* canvas;
    CMStageDeclarativeView* view;

    QString file;

    void updateCanvas();
};

CMStageCanvas::CMStageCanvas(QDeclarativeItem* parent)
    : CMCanvasControllerDeclarative(parent), d(new Private(this))
{

}

CMStageCanvas::~CMStageCanvas()
{
    delete d;
}

QObject* CMStageCanvas::doc() const
{
    return d->doc;
}

QString CMStageCanvas::file() const
{
    return d->file;
}

void CMStageCanvas::setFile(const QString& file)
{
    d->file = file;
}

void CMStageCanvas::loadDocument()
{
    emit progress(1);
    KPrDocument* doc = new KPrDocument(0, 0);
    d->doc = doc;
    d->updateCanvas();

    CMProgressProxy *proxy = new CMProgressProxy(this);
    doc->setProgressProxy(proxy);

    connect(proxy, SIGNAL(valueChanged(int)), SIGNAL(progress(int)));

    if(!doc->openUrl(KUrl(d->file))) {
        kWarning() << "Could not open file:" << d->file;
        return;
    }

    setMargin(10);
    d->view->setActivePage(d->doc->pageByIndex(0, false));
    d->updateCanvas();

    emit progress(100);
    emit completed();
}

void CMStageCanvas::Private::updateCanvas()
{
    if (canvas && canvas->document() != doc) {
        delete canvas;
        canvas = 0;
    }

    if (!canvas && doc != 0) {
        canvas = static_cast<KoPACanvasItem*>(doc->canvasItem());
        canvas->setCanvasController(q);
        view = new CMStageDeclarativeView( q->zoomController(new KoZoomHandler()), doc, canvas );
        q->setCanvas(canvas);
        connect(q->proxyObject, SIGNAL(moveDocumentOffset(const QPoint&)), canvas, SLOT(slotSetDocumentOffset(QPoint)));
        connect(canvas, SIGNAL(documentSize(const QSize&)), q->proxyObject, SLOT(sizeChanged(const QSize&)));
    }

    canvas->updateCanvas(QRectF(0, 0, q->width(), q->height()));
}

