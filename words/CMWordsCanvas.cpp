#include "CMWordsCanvas.h"

#include <KDE/KDebug>

#include <KoZoomHandler.h>
#include <KoZoomController.h>

#include <part/KWDocument.h>
#include <part/KWCanvasItem.h>
#include <part/KWViewModeNormal.h>
#include <KoProgressUpdater.h>
#include <CMProgressProxy.h>
#include <QTimer>

class CMWordsCanvas::Private
{
public:
    Private(CMWordsCanvas* qq)
        : q(qq), doc(0), canvas(0)
    { }
    ~Private() { }

    CMWordsCanvas* q;

    KWDocument* doc;
    KWCanvasItem* canvas;

    void updateCanvas();
};

CMWordsCanvas::CMWordsCanvas(QDeclarativeItem* parent)
    : CMCanvasControllerDeclarative(parent), d(new Private(this))
{

}

CMWordsCanvas::~CMWordsCanvas()
{
    delete d;
}

QObject* CMWordsCanvas::doc() const
{
    return d->doc;
}

void CMWordsCanvas::changePage(int newPage)
{
    KWPage thePage = d->doc->pageManager()->page(newPage + 1);
    scrollContentsBy( 0, thePage.offsetInDocument() - documentOffset().y());
}

void CMWordsCanvas::loadDocument()
{
    emit progress(1);

    KWDocument* doc = new KWDocument();
    d->doc = doc;

    CMProgressProxy *proxy = new CMProgressProxy(this);
    doc->setProgressProxy(proxy);

    connect(proxy, SIGNAL(valueChanged(int)), SIGNAL(progress(int)));

    setMargin(10);

    if(!doc->openUrl(KUrl(file()))) {
        kWarning() << "Could not open file:" << file();
        return;
    }

    d->updateCanvas();

    emit progress(100);
    emit completed();
}

void CMWordsCanvas::Private::updateCanvas()
{
    if (canvas && canvas->document() != doc) {
        delete canvas;
        canvas = 0;
    }

    if (!canvas && doc != 0) {
        canvas = static_cast<KWCanvasItem*>(doc->canvasItem());
        q->setCanvas(canvas);
        connect(q->proxyObject, SIGNAL(moveDocumentOffset(const QPoint&)), canvas, SLOT(setDocumentOffset(QPoint)));
        connect(canvas, SIGNAL(documentSize(QSizeF)), q->zoomController(), SLOT(setDocumentSize(QSizeF)));
        canvas->updateSize();
    }

    canvas->updateCanvas(QRectF(0, 0, q->width(), q->height()));
}

#include "CMWordsCanvas.moc"
