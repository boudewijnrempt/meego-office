#include "CMWordsCanvas.h"

#include "CMCanvasInputProxy.h"

#include <KDE/KDebug>

#include <KoZoomHandler.h>
#include <KoZoomController.h>

#include <part/KWDocument.h>
#include <part/KWCanvasItem.h>
#include <part/KWViewModeNormal.h>
#include <part/KWCanvasBase.h>
#include <KoProgressUpdater.h>
#include <CMProgressProxy.h>
#include <QTimer>
#include <KoFindText.h>
#include <QTextCursor>
#include <KoShapeManager.h>
#include <KoShape.h>
#include <KoTextShapeData.h>
#include <KoSelection.h>
#include <KoToolProxy.h>

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
    void update();

    CMWordsCanvas* q;

    KWDocument* doc;
    KWCanvasItem* canvas;
    int currentPage;

    KoFindText* find;
    int matchNumber;

    void updateCanvas();
};

CMWordsCanvas::CMWordsCanvas(QDeclarativeItem* parent)
    : CMCanvasControllerDeclarative(parent), d(new Private(this))
{
    CMProcessInputInterface::setupConnections(inputProxy(), this);
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
    if(newPage < 0)
        newPage = pageCount() - 1;

    if(newPage >= pageCount())
        newPage = 0;

    KWCanvasBase* canvasItem = dynamic_cast<KWCanvasBase *>(canvas()->canvasItem());
    if(canvasItem) {
        KWPage thePage = d->doc->pageManager()->page(newPage + 1);
        QPointF pos = d->canvas->viewConverter()->documentToView(QPointF(0, thePage.offsetInDocument()));
        scrollContentsBy( 0, pos.y() - documentOffset().y());
        d->currentPage = newPage;
        emit pageChanged(newPage);
    }
}

bool CMWordsCanvas::hasNextPage() const
{
    int pc = pageCount();
    if(pc == 1)
        return false;
    if(d->currentPage < pc - 1) {
        return true;
    }
    return false;
}

bool CMWordsCanvas::hasPreviousPage() const
{
    if(d->currentPage > 0) {
        return true;
    }
    return false;
}

void CMWordsCanvas::goToNextPage()
{
    if(hasNextPage())
        changePage(d->currentPage + 1);
}

void CMWordsCanvas::goToPreviousPage()
{
    if(hasPreviousPage())
        changePage(d->currentPage - 1);
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

    CMProgressProxy *proxy = new CMProgressProxy(this);
    doc->setProgressProxy(proxy);
    connect(proxy, SIGNAL(valueChanged(int)), SIGNAL(progress(int)));

    setMargin(10);

    if(!doc->openUrl(KUrl(file()))) {
        kWarning() << "Could not open file:" << file();
        return;
    }

    d->updateCanvas();

    QList<QTextDocument*> texts;
    KoFindText::findTextInShapes(d->canvas->shapeManager()->shapes(), texts);

    d->find = new KoFindText(texts, this);
    connect(d->find, SIGNAL(matchFound(KoFindMatch)), this, SLOT(matchFound(KoFindMatch)));
    connect(d->find, SIGNAL(updateCanvas()), this, SLOT(update()));

    emit progress(100);
    emit completed();
    emit pageChanged(0);
}

int CMWordsCanvas::pageCount() const
{
    return d->doc->pageCount();
}

int CMWordsCanvas::matchCount()
{
    return d->find->matches().count();
}

void CMWordsCanvas::find(const QString& pattern)
{
    d->matchNumber = 0;
    d->find->find(pattern);
}

void CMWordsCanvas::findPrevious()
{
    d->find->findPrevious();
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

    matchNumber = find->matches().indexOf(match) + 1;
    emit q->findMatchFound(matchNumber);

    QTextCursor cursor = match.location().value<QTextCursor>();
    QTextLine line = cursor.block().layout()->lineForTextPosition(cursor.position() - cursor.block().position());
    QRectF textRect(line.cursorToX(cursor.anchor() - cursor.block().position()) , line.y(), 1, line.height());
    q->ensureVisible(canvas->viewConverter()->documentToView(textRect), false);
}

void CMWordsCanvas::Private::update()
{
    canvas->updateCanvas(QRectF(QPointF(0.f, 0.f), canvas->canvasItem()->size()));
}


void CMWordsCanvas::onSingleTap( const QPointF& location )
{
    KWCanvasBase *kwcanvasitem = dynamic_cast<KWCanvasBase *>(canvas()->canvasItem());
    KoShapeManager *shapeManager = kwcanvasitem->shapeManager();

    // select the shape under the current position and then activate the text tool, send mouse events
    QPointF pos = canvas()->canvasItem()->mapFromScene(location);

    // get the current location in document coordinates
    QPointF docPos = d->canvas->viewConverter()->viewToDocument(pos + scrollBarValue() - canvas()->canvasItem()->pos());

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

void CMWordsCanvas::onDoubleTap ( const QPointF& location )
{
    Q_UNUSED(location);
    emit enterFullScreen();
}

void CMWordsCanvas::onLongTap ( const QPointF& location )
{
    //Do selection stuff
}

#include "CMWordsCanvas.moc"
