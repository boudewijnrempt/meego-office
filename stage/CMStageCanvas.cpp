#include "CMStageCanvas.h"

#include <QtGui/QGraphicsItem>
#include <KDE/KDebug>
#include <KoPACanvasItem.h>
#include <KoCanvasBase.h>
#include <part/KPrDocument.h>
#include <part/KPrPage.h>
#include <part/KPrNotes.h>
#include <part/KPrView.h>
#include "CMStageDeclarativeView.h"
#include "CMProgressProxy.h"
#include <KoPAPageBase.h>
#include <KoZoomController.h>
#include <KoShapeManager.h>
#include <KoToolManager.h>
#include <KoToolProxy.h>
#include <KoSelection.h>
#include <KoFindText.h>
#include <KoFindMatch.h>
#include <QTextCursor>
#include <QTextLine>
#include <QTextBlock>
#include <KoTextShapeData.h>
#include <KoTextDocument.h>
#include <KoTextEditor.h>
#include <KoTextLayoutRootArea.h>

class CMStageCanvas::Private
{
public:
    Private(CMStageCanvas* qq)
        : q(qq), doc(0), canvas(0), matchNumber(0)
    { }
    ~Private() { }

    void updateCanvas();
    void setDocumentSize(const QSize &size);
    void matchFound(KoFindMatch match);
    void update();
    void updatePanGesture(const QPointF &location);
    void moveSelectionHandles();

    CMStageCanvas* q;

    KPrDocument* doc;
    KoPACanvasItem* canvas;
    CMStageDeclarativeView* view;

    KoFindText *finder;
    int matchNumber;
};

CMStageCanvas::CMStageCanvas(QDeclarativeItem* parent)
    : CMCanvasControllerDeclarative(parent), CMTextSelection(this), d(new Private(this))
{
    CMProcessInputInterface::setupConnections(inputProxy(), this);
    connect(inputProxy(), SIGNAL(updatePanGesture(QPointF)), this, SLOT(updatePanGesture(QPointF)));

    KoZoomMode::setMinimumZoom(0.5);
    KoZoomMode::setMaximumZoom(2.0);

    connect(proxyObject, SIGNAL(moveDocumentOffset(QPoint)), this, SLOT(moveSelectionHandles()));
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

int CMStageCanvas::matchCount()
{
    return d->finder->matches().count();
}

void CMStageCanvas::copySelection()
{
    CMTextSelection::copySelection();
}

void CMStageCanvas::changeSlide(int newSlide)
{
    if(newSlide < 0)
        newSlide = slideCount() - 1;

    if(newSlide >= slideCount())
        newSlide = 0;

    d->view->setPage(newSlide);
    emit slideChanged(newSlide);
    emit currentPageNotesChanged();
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

    QList<QTextDocument*> texts;
    QList<KoPAPageBase*> pages = doc->pages();
    foreach(KoPAPageBase* page, pages) {
        KoFindText::findTextInShapes(page->shapes(), texts);
    }
    d->finder = new KoFindText(texts, this);
    connect(d->finder, SIGNAL(matchFound(KoFindMatch)), this, SLOT(matchFound(KoFindMatch)));
    connect(d->finder, SIGNAL(updateCanvas()), this, SLOT(update()));

    emit progress(100);
    emit completed();
    emit slideChanged(0);
}

void CMStageCanvas::find ( const QString& pattern )
{
    d->finder->find(pattern);
}

void CMStageCanvas::findFinished()
{
    d->finder->finished();
}

void CMStageCanvas::findNext()
{
    d->finder->findNext();
}

void CMStageCanvas::findPrevious()
{
    d->finder->findPrevious();
}

void CMStageCanvas::setSelectionAnchorHandle(QDeclarativeItem* handle)
{
    CMCanvasControllerDeclarative::setSelectionAnchorHandle(handle);
    setAnchorHandle(handle);
    connect(handle, SIGNAL(xChanged()), this, SLOT(updateFromHandles()));
    connect(handle, SIGNAL(yChanged()), this, SLOT(updateFromHandles()));
}

void CMStageCanvas::setSelectionCursorHandle(QDeclarativeItem* handle)
{
    CMCanvasControllerDeclarative::setSelectionCursorHandle(handle);
    setPositionHandle(handle);
    connect(handle, SIGNAL(xChanged()), this, SLOT(updateFromHandles()));
    connect(handle, SIGNAL(yChanged()), this, SLOT(updateFromHandles()));
}

QString CMStageCanvas::currentPageNotes() const
{
    if(d->doc)
    {
        QList<KoPAPageBase*> pages = d->doc->pages();
        if(d->view->page() < pages.count())
        {
            KPrPage* thePage = dynamic_cast<KPrPage*>( pages.at(slide()) );
            if(thePage)
            {
                QList<QTextDocument*> texts;
                KoFindText::findTextInShapes(thePage->pageNotes()->shapes(), texts);
                QString noteText;
                foreach(QTextDocument* doc, texts) {
                    noteText.append(doc->toHtml());
                }
                return noteText;
            }
        }
    }
    return QLatin1String("page 1");
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
        view = new CMStageDeclarativeView( q->zoomController(), doc, canvas);
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

void CMStageCanvas::onSingleTap(const QPointF &location)
{
    KoCanvasBase *canvasItem = dynamic_cast<KoCanvasBase *>(canvas()->canvasItem());
    KoShapeManager *shapeManager = canvasItem->shapeManager();

    // select the shape under the current position and then activate the text tool, send mouse events
    QPointF pos = canvas()->canvasItem()->mapFromScene(location);

    // get the current location in document coordinates
    QPointF docPos = (canvasItem->viewConverter()->viewToDocument(pos + scrollBarValue() - canvas()->canvasItem()->pos()));

    // find text shape at current position
    KoShape *shape = shapeManager->shapeAt(docPos);

    if (!shape || shape->shapeId() != "TextShapeID") return;

    KoSelection *selection = shapeManager->selection();
    if (!selection) return;
    selection->select(shape);
    setHasSelection(false);
    selectionAnchorHandle()->setVisible(false);
    selectionCursorHandle()->setVisible(false);

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
    canvas()->toolProxy()->mouseReleaseEvent(&release, canvas()->viewConverter()->viewToDocument(pos + documentOffset()));
}

void CMStageCanvas::onDoubleTap ( const QPointF& location )
{

}

void CMStageCanvas::onLongTap ( const QPointF& location )
{
   KoToolManager::instance()->switchToolRequested("TextToolFactory_ID");

    updatePosition(UpdatePosition, location);
    KoTextShapeData * shapeData = textShapeDataForPosition(location);
    if(!shapeData) {
        return;
    }

    KoTextEditor *editor = KoTextDocument(shapeData->document()).textEditor();
    if(editor->hasSelection()) {
        editor->select(QTextCursor::WordUnderCursor);
        d->canvas->updateCanvas(shapeData->rootArea()->associatedShape()->boundingRect());
        updateHandlePositions(*(editor->cursor()));
    }
}

void CMStageCanvas::onLongTapEnd(const QPointF& location)
{
    if(hasSelection()) {
        emit selected(location);
    }
}

QPointF CMStageCanvas::documentToView(const QPointF& point)
{
    return d->canvas->viewConverter()->documentToView(point);
}

QPointF CMStageCanvas::viewToDocument(const QPointF& point)
{
    return d->canvas->viewConverter()->viewToDocument(point);
}

void CMStageCanvas::updateFromHandles()
{
    CMTextSelection::updateFromHandles();
}

void CMStageCanvas::Private::matchFound ( KoFindMatch match )
{
    matchNumber = finder->matches().indexOf(match) + 1;
    emit q->findMatchFound(matchNumber);

    QTextCursor cursor = match.location().value<QTextCursor>();
    QTextLine line = cursor.block().layout()->lineForTextPosition(cursor.position() - cursor.block().position());
    QRectF textRect(line.cursorToX(cursor.anchor() - cursor.block().position()) , line.y(), 1, line.height());
    q->ensureVisible(canvas->viewConverter()->documentToView(textRect), false);
}

void CMStageCanvas::Private::update()
{
    canvas->updateCanvas(QRectF(0.0f, 0.0f, q->width(), q->height()));
}

void CMStageCanvas::Private::updatePanGesture(const QPointF& location)
{
    q->updatePosition(CMTextSelection::UpdatePosition, location);
}

void CMStageCanvas::Private::moveSelectionHandles()
{
    q->updateHandlePositions();
}

#include "CMStageCanvas.moc"
