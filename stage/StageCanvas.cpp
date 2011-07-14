#include "StageCanvas.h"

#include <QtGui/QGraphicsItem>
#include <QtGui/QTextCursor>
#include <QtGui/QTextLine>
#include <QtGui/QTextBlock>

#include <KDE/KDebug>

#include <KoCanvasBase.h>
#include <KoZoomController.h>
#include <KoShapeManager.h>
#include <KoToolManager.h>
#include <KoToolProxy.h>
#include <KoSelection.h>
#include <KoFindText.h>
#include <KoFindMatch.h>
#include <KoTextShapeData.h>
#include <KoTextDocument.h>
#include <KoTextEditor.h>
#include <KoTextLayoutRootArea.h>

#include <KoPAPageBase.h>
#include <KoPACanvasItem.h>

#include <stage/part/KPrDocument.h>
#include <stage/part/KPrPage.h>
#include <stage/part/KPrNotes.h>
#include <stage/part/KPrView.h>

#include "StageDeclarativeView.h"
#include "shared/ProgressProxy.h"

class StageCanvas::Private
{
public:
    Private( StageCanvas* qq)
        : q(qq), doc(0), canvas(0), matchNumber(0)
    { }
    ~Private() { }

    void updateCanvas();
    void setDocumentSize(const QSize &size);
    void matchFound(KoFindMatch match);
    void update();
    void updatePanGesture(const QPointF &location);
    void moveSelectionHandles();

    StageCanvas* q;

    KPrDocument* doc;
    KoPACanvasItem* canvas;
    StageDeclarativeView* view;

    KoFindText *finder;
    int matchNumber;
};

StageCanvas::StageCanvas(QDeclarativeItem* parent)
    : CanvasControllerDeclarative(parent), TextSelection(this), d(new Private(this))
{
    ProcessInputInterface::setupConnections(inputProxy(), this);
    connect(inputProxy(), SIGNAL(updatePanGesture(QPointF)), this, SLOT(updatePanGesture(QPointF)));

    KoZoomMode::setMinimumZoom(0.5);
    KoZoomMode::setMaximumZoom(2.0);

    connect(proxyObject, SIGNAL(moveDocumentOffset(QPoint)), this, SLOT(moveSelectionHandles()));
}

StageCanvas::~StageCanvas()
{
    delete d;
}

QObject* StageCanvas::doc() const
{
    return d->doc;
}

QObject* StageCanvas::view() const
{
    return d->view;
}

int StageCanvas::slide() const
{
    return d->view->page();
}

int StageCanvas::slideCount() const
{
    return d->doc->pageCount();
}

int StageCanvas::matchCount()
{
    return d->finder->matches().count();
}

void StageCanvas::copySelection()
{
    TextSelection::copySelection();
}

void StageCanvas::changeSlide(int newSlide)
{
    if(newSlide < 0)
        newSlide = slideCount() - 1;

    if(newSlide >= slideCount())
        newSlide = 0;

    d->view->setPage(newSlide);
    emit slideChanged(newSlide);
    emit currentPageNotesChanged();
}

void StageCanvas::loadDocument()
{
    emit progress(1);
    KPrDocument* doc = new KPrDocument(0, 0);
    d->doc = doc;

    ProgressProxy *proxy = new ProgressProxy(this);
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

void StageCanvas::find ( const QString& pattern )
{
    d->finder->find(pattern);
}

void StageCanvas::findFinished()
{
    d->finder->finished();
}

void StageCanvas::findNext()
{
    d->finder->findNext();
}

void StageCanvas::findPrevious()
{
    d->finder->findPrevious();
}

void StageCanvas::setSelectionAnchorHandle(QDeclarativeItem* handle)
{
    CanvasControllerDeclarative::setSelectionAnchorHandle(handle);
    setAnchorHandle(handle);
    connect(handle, SIGNAL(xChanged()), this, SLOT(updateFromHandles()));
    connect(handle, SIGNAL(yChanged()), this, SLOT(updateFromHandles()));
}

void StageCanvas::setSelectionCursorHandle(QDeclarativeItem* handle)
{
    CanvasControllerDeclarative::setSelectionCursorHandle(handle);
    setPositionHandle(handle);
    connect(handle, SIGNAL(xChanged()), this, SLOT(updateFromHandles()));
    connect(handle, SIGNAL(yChanged()), this, SLOT(updateFromHandles()));
}

QString StageCanvas::currentPageNotes() const
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
                    noteText.append(doc->toPlainText());
                }
                return noteText;
            }
        }
    }
    return QLatin1String("page 1");
}

void StageCanvas::Private::updateCanvas()
{
    if (canvas && canvas->document() != doc) {
        delete canvas;
        canvas = 0;
    }

    if (!canvas && doc != 0) {
        canvas = static_cast<KoPACanvasItem*>(doc->canvasItem());
        canvas->setCanvasController(q);
        view = new StageDeclarativeView( q->zoomController(), doc, canvas);
        q->setCanvas(canvas);
        connect(q->proxyObject, SIGNAL(moveDocumentOffset(const QPoint&)), canvas, SLOT(slotSetDocumentOffset(QPoint)));
        connect(canvas, SIGNAL(documentSize(const QSize&)), q, SLOT(setDocumentSize(const QSize&)));
        canvas->updateSize();
        q->resetDocumentOffset();
    }

    canvas->updateCanvas(QRectF(0, 0, q->width(), q->height()));
}

void StageCanvas::Private::setDocumentSize(const QSize& size)
{
    q->zoomController()->setDocumentSize(canvas->viewConverter()->viewToDocument(size));
}

void StageCanvas::onSingleTap(const QPointF &location)
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

void StageCanvas::onDoubleTap ( const QPointF& location )
{

}

void StageCanvas::onLongTap ( const QPointF& location )
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

void StageCanvas::onLongTapEnd(const QPointF& location)
{
    if(hasSelection()) {
        emit selected(location);
    }
}

QPointF StageCanvas::documentToView(const QPointF& point)
{
    return d->canvas->viewConverter()->documentToView(point);
}

QPointF StageCanvas::viewToDocument(const QPointF& point)
{
    return d->canvas->viewConverter()->viewToDocument(point);
}

void StageCanvas::updateFromHandles()
{
    TextSelection::updateFromHandles();
}

void StageCanvas::Private::matchFound ( KoFindMatch match )
{
    matchNumber = finder->matches().indexOf(match) + 1;
    emit q->findMatchFound(matchNumber);

    QTextCursor cursor = match.location().value<QTextCursor>();
    QTextLine line = cursor.block().layout()->lineForTextPosition(cursor.position() - cursor.block().position());
    QRectF textRect(line.cursorToX(cursor.anchor() - cursor.block().position()) , line.y(), 1, line.height());
    q->ensureVisible(canvas->viewConverter()->documentToView(textRect), false);
}

void StageCanvas::Private::update()
{
    canvas->updateCanvas(QRectF(0.0f, 0.0f, q->width(), q->height()));
}

void StageCanvas::Private::updatePanGesture(const QPointF& location)
{
    q->updatePosition( TextSelection::UpdatePosition, location);
}

void StageCanvas::Private::moveSelectionHandles()
{
    q->updateHandlePositions();
}

#include "StageCanvas.moc"
