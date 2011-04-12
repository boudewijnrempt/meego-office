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
#include <KoFindText.h>
#include <QTextCursor>
#include <KoShapeManager.h>
#include <KoShape.h>
#include <KoTextShapeData.h>
#include <QTextLine>
#include <QTextBlock>

class CMWordsCanvas::Private
{
public:
    Private(CMWordsCanvas* qq)
        : q(qq), doc(0), canvas(0), currentPage(0)
    { }
    ~Private() { }

    void matchFound(KoFindMatch match);

    CMWordsCanvas* q;

    KWDocument* doc;
    KWCanvasItem* canvas;
    int currentPage;

    KoFindText* find;

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
    d->currentPage = newPage;
    emit pageChanged(newPage);
}

int CMWordsCanvas::page() const
{
    return d->doc->pageManager()->page(documentOffset()).pageNumber();
}

void CMWordsCanvas::loadDocument()
{
    qDebug() << "Loading words document";
    emit progress(1);

    KWDocument* doc = new KWDocument();
    d->doc = doc;

    d->find = new KoFindText(doc->resourceManager(), this);
    connect(d->find, SIGNAL(matchFound(KoFindMatch)), this, SLOT(matchFound(KoFindMatch)));

    CMProgressProxy *proxy = new CMProgressProxy(this);
    doc->setProgressProxy(proxy);
    connect(proxy, SIGNAL(valueChanged(int)), SIGNAL(progress(int)));

    setMargin(10);

    if(!doc->openUrl(KUrl(file()))) {
        kWarning() << "Could not open file:" << file();
        return;
    }

    d->updateCanvas();

    connect(d->find, SIGNAL(updateCanvas()), this, SLOT(update()));

    QList<KoShape*> shapes = canvas()->shapeManager()->shapes();
    foreach(KoShape* shape, shapes) {
        KoTextShapeData *shapeData = dynamic_cast<KoTextShapeData *>(shape->userData());
        if (shapeData == 0)
            continue;

        if(shapeData->document()) {
            QVariant var = QVariant::fromValue<void*>(shapeData->document());
            doc->resourceManager()->setResource(KoText::CurrentTextDocument, var);
        }
    }

    emit progress(100);
    emit completed();
}

void CMWordsCanvas::find(const QString& pattern)
{
    d->find->find(pattern);
}

void CMWordsCanvas::findNext()
{
    d->find->findNext();
}

void CMWordsCanvas::findFinished()
{
    d->find->finished();
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
    
    // This is horrendously expensive - please let there be a better way...
    int theCurrentPage = q->page();
    if(theCurrentPage != currentPage) {
        currentPage = theCurrentPage;
        emit q->pageChanged(theCurrentPage);
    }
}

void CMWordsCanvas::Private::matchFound(KoFindMatch match)
{
    if(!match.isValid()) {
        return;
    }

    QTextCursor cursor = match.location().value<QTextCursor>();
    doc->resourceManager()->setResource(KoText::CurrentTextAnchor, cursor.anchor());
    doc->resourceManager()->setResource(KoText::CurrentTextPosition, cursor.position());

    QTextLine line = cursor.block().layout()->lineForTextPosition(cursor.position() - cursor.block().position());
    QRectF textRect(q->documentOffset().x(), line.y(), 1, line.height());
    q->ensureVisible(textRect, true);

    canvas->updateCanvas(QRectF(QPointF(0.f, 0.f), canvas->canvasItem()->size()));
}

#include "CMWordsCanvas.moc"
