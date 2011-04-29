#include "CMStageCanvas.h"

#include <QtGui/QGraphicsItem>
#include <KDE/KDebug>
#include <KoPACanvasItem.h>
#include <KoCanvasBase.h>
#include <part/KPrDocument.h>
#include <part/KPrView.h>
#include "CMStageDeclarativeView.h"
#include "CMProgressProxy.h"
#include <KoPAPageBase.h>
#include <KoZoomController.h>
#include <KoShapeManager.h>
#include <KoToolManager.h>
#include <KoToolProxy.h>
#include <KoSelection.h>

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

    void updateCanvas();
    void setDocumentSize(const QSize &size);
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

QObject* CMStageCanvas::view() const
{
    return d->view;
}

int CMStageCanvas::slide() const
{
    return d->view->page();
}

int CMStageCanvas::slideCount() const
{
    return d->doc->pageCount();
}

void CMStageCanvas::changeSlide(int newSlide)
{
    if(newSlide < 0)
        newSlide = slideCount() - 1;

    if(newSlide >= slideCount())
        newSlide = 0;

    d->view->setPage(newSlide);
    emit slideChanged(newSlide);
}

void CMStageCanvas::loadDocument()
{
    emit progress(1);
    KPrDocument* doc = new KPrDocument(0, 0);
    d->doc = doc;

    CMProgressProxy *proxy = new CMProgressProxy(this);
    doc->setProgressProxy(proxy);

    connect(proxy, SIGNAL(valueChanged(int)), SIGNAL(progress(int)));

    if(!doc->openUrl(KUrl(file()))) {
        kWarning() << "Could not open file:" << file();
        return;
    }

    setMargin(10);
    d->updateCanvas();
    d->view->setActivePage(d->doc->pageByIndex(0, false));

    emit progress(100);
    emit completed();
    emit slideChanged(0);
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
        view = new CMStageDeclarativeView( q->zoomController(new KoZoomHandler()), doc, canvas);
        q->setCanvas(canvas);
        connect(q->proxyObject, SIGNAL(moveDocumentOffset(const QPoint&)), canvas, SLOT(slotSetDocumentOffset(QPoint)));
        connect(canvas, SIGNAL(documentSize(const QSize&)), q, SLOT(setDocumentSize(const QSize&)));
        canvas->updateSize();
    }

    canvas->updateCanvas(QRectF(0, 0, q->width(), q->height()));
}

void CMStageCanvas::Private::setDocumentSize(const QSize& size)
{
    q->zoomController()->setDocumentSize(size);
}

void CMStageCanvas::handleShortTap(QPointF pos)
{
    KoCanvasBase *canvasItem = dynamic_cast<KoCanvasBase *>(canvas()->canvasItem());
    KoShapeManager *shapeManager = canvasItem->shapeManager();

    // select the shape under the current position and then activate the text tool, send mouse events
    pos = canvas()->canvasItem()->mapFromScene(pos);

    // get the current location in document coordinates
    QPointF docPos = (canvasItem->viewConverter()->viewToDocument(pos + scrollBarValue() - canvas()->canvasItem()->pos()));

    // find text shape at current position
    KoShape *shape = shapeManager->shapeAt(docPos);

    if (!shape || shape->shapeId() != "TextShapeID") return;

    KoSelection *selection = shapeManager->selection();
    if (!selection) return;
    selection->select(shape);

    // The text tool is responsible for handling clicks...
    KoToolManager::instance()->switchToolRequested("TextToolFactory_ID");

    // Click...
    QMouseEvent press(QEvent::MouseButtonPress,
                      pos.toPoint(),
                      Qt::LeftButton,
                      Qt::LeftButton,
                      Qt::NoModifier);
    canvas()->toolProxy()->mousePressEvent(&press, canvas()->viewConverter()->viewToDocument(pos + documentOffset()));


    // And release...
    QMouseEvent release(QEvent::MouseButtonRelease,
                        pos.toPoint(),
                        Qt::LeftButton,
                        Qt::LeftButton,
                        Qt::NoModifier);
    canvas()->toolProxy()->mousePressEvent(&release, canvas()->viewConverter()->viewToDocument(pos + documentOffset()));

}

#include "CMStageCanvas.moc"
