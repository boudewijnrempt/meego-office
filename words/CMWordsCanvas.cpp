#include "CMWordsCanvas.h"

#include <KDE/KDebug>

#include <KoZoomHandler.h>
#include <KoZoomController.h>

#include <part/KWDocument.h>
#include <part/KWCanvasItem.h>
#include <part/KWViewModeNormal.h>

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

    QString file;

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

QString CMWordsCanvas::file() const
{
    return d->file;
}

void CMWordsCanvas::setFile(const QString& file)
{
    KWDocument* doc = new KWDocument();
    d->file = file;
    d->doc = doc;

    setMargin(10);

    if(!doc->openUrl(KUrl(file))) {
        kWarning() << "Could not open file:" << file;
        return;
    }

    d->updateCanvas();
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
    }

    canvas->updateCanvas(QRectF(0, 0, q->width(), q->height()));
}
