#include "WordsCanvas.h"

#include <QtCore/QTimer>
#include <QtGui/QTextLine>
#include <QtGui/QTextBlock>
#include <QtGui/QTextCursor>

#include <KDE/KDebug>

#include <KoZoomHandler.h>
#include <KoZoomController.h>
#include <KoProgressUpdater.h>
#include <KoFindText.h>
#include <KoShapeManager.h>
#include <KoShape.h>
#include <KoTextShapeData.h>
#include <KoSelection.h>
#include <KoToolProxy.h>
#include <KoTextLayoutRootArea.h>
#include <KoTextEditor.h>

#include <words/part/KWDocument.h>
#include <words/part/KWCanvasItem.h>
#include <words/part/KWViewModeNormal.h>
#include <words/part/KWCanvasBase.h>

#include "shared/ProgressProxy.h"
#include "shared/CanvasInputProxy.h"

class WordsCanvas::Private
{
public:
    Private( WordsCanvas* qq)
        : q(qq), doc(0), canvas(0), currentPage(0)
    { }
    ~Private() { }

    void matchFound(KoFindMatch match);
    void update();
    void updateCanvas();
    void updatePanGesture(const QPointF &location);
    void documentOffsetMoved(QPoint newOffset);

    WordsCanvas* q;

    KWDocument* doc;
    KWCanvasItem* canvas;
    int currentPage;

    KoFindText* find;
    int matchNumber;
};

WordsCanvas::WordsCanvas(QDeclarativeItem* parent)
    : CanvasControllerDeclarative(parent), TextSelection(this), d(new Private(this))
{
    ProcessInputInterface::setupConnections(inputProxy(), this);
    connect(inputProxy(), SIGNAL(updatePanGesture(QPointF)), SLOT(updatePanGesture(QPointF)));
    KoZoomMode::setMinimumZoom(0.5);
    KoZoomMode::setMaximumZoom(2.0);
    connect(proxyObject, SIGNAL(moveDocumentOffset(QPoint)), this, SLOT(documentOffsetMoved(QPoint)));
}

WordsCanvas::~WordsCanvas()
{
    delete d;
}

QObject* WordsCanvas::doc() const
{
    return d->doc;
}

void WordsCanvas::changePage(int newPage)
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

bool WordsCanvas::hasNextPage() const
{
    int pc = pageCount();
    if(pc == 1)
        return false;
    if(d->currentPage < pc - 1) {
        return true;
    }
    return false;
}

bool WordsCanvas::hasPreviousPage() const
{
    if(d->currentPage > 0) {
        return true;
    }
    return false;
}

void WordsCanvas::goToNextPage()
{
    if(hasNextPage())
        changePage(d->currentPage + 1);
    else
        changePage(0);
}

void WordsCanvas::goToPreviousPage()
{
    if(hasPreviousPage())
        changePage(d->currentPage - 1);
    else
        changePage(pageCount() - 1);
}

int WordsCanvas::page() const
{
    return d->doc->pageManager()->page(documentOffset()).pageNumber();
}

void WordsCanvas::loadDocument()
{
    qDebug() << "Loading words document";
    emit progress(1);

    KWDocument* doc = new KWDocument();
    d->doc = doc;
    doc->setAutoSave(0);

    ProgressProxy *proxy = new ProgressProxy(this);
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

int WordsCanvas::pageCount() const
{
    return d->doc->pageCount();
}

int WordsCanvas::matchCount()
{
    return d->find->matches().count();
}

void WordsCanvas::copySelection()
{
    TextSelection::copySelection();
}

void WordsCanvas::find(const QString& pattern)
{
    d->matchNumber = 0;
    d->find->find(pattern);
}

void WordsCanvas::findPrevious()
{
    d->find->findPrevious();
}

void WordsCanvas::findNext()
{
    d->find->findNext();
}

void WordsCanvas::findFinished()
{
    d->find->finished();
}

void WordsCanvas::setSelectionAnchorHandle(QDeclarativeItem* handle)
{
    CanvasControllerDeclarative::setSelectionAnchorHandle(handle);
    setAnchorHandle(handle);
    connect(handle, SIGNAL(xChanged()), this, SLOT(updateFromHandles()));
    connect(handle, SIGNAL(yChanged()), this, SLOT(updateFromHandles()));
}

void WordsCanvas::setSelectionCursorHandle(QDeclarativeItem* handle)
{
    CanvasControllerDeclarative::setSelectionCursorHandle(handle);
    setPositionHandle(handle);
    connect(handle, SIGNAL(xChanged()), this, SLOT(updateFromHandles()));
    connect(handle, SIGNAL(yChanged()), this, SLOT(updateFromHandles()));
}

void WordsCanvas::Private::updateCanvas()
{
    if (canvas && canvas->document() != doc) {
        delete canvas;
        canvas = 0;
    }

    if (!canvas && doc != 0) {
        canvas = static_cast<KWCanvasItem*>(doc->canvasItem());
	canvas->setCacheEnabled(true);
        q->setCanvas(canvas);
        connect(q->proxyObject, SIGNAL(moveDocumentOffset(const QPoint&)), canvas, SLOT(setDocumentOffset(QPoint)));
        connect(canvas, SIGNAL(documentSize(QSizeF)), q->zoomController(), SLOT(setDocumentSize(QSizeF)));
        canvas->updateSize();
        q->resetDocumentOffset();
    }

    canvas->updateCanvas(QRectF(0, 0, q->width(), q->height()));

    // This is horrendously expensive - please let there be a better way...
    int theCurrentPage = q->page();
    if(theCurrentPage != currentPage) {
        currentPage = theCurrentPage;
        emit q->pageChanged(theCurrentPage);
    }
}

void WordsCanvas::Private::matchFound(KoFindMatch match)
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

void WordsCanvas::Private::update()
{
    canvas->updateCanvas(canvas->viewMode()->viewToDocument(QRectF(q->x(), q->y(), q->width(), q->height()).translated(q->getDocumentOffset()), canvas->viewConverter()));
}

void WordsCanvas::onSingleTap( const QPointF& location )
{
    KoShapeManager *shapeManager = d->canvas->shapeManager();

    QPointF docPos = d->canvas->viewMode()->viewToDocument(location + documentOffset(), canvas()->viewConverter());

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
                      location.toPoint(),
                      Qt::LeftButton,
                      Qt::LeftButton,
                      Qt::NoModifier);
    canvas()->toolProxy()->mousePressEvent(&press, docPos);


    // And release...
    QMouseEvent release(QEvent::MouseButtonRelease,
                        location.toPoint(),
                        Qt::LeftButton,
                        Qt::LeftButton,
                        Qt::NoModifier);
    canvas()->toolProxy()->mouseReleaseEvent(&release, docPos);
}

void WordsCanvas::onDoubleTap ( const QPointF& location )
{
    Q_UNUSED(location);
    emit doubleTapped();
}

void WordsCanvas::onLongTap ( const QPointF& location )
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

void WordsCanvas::onLongTapEnd(const QPointF &location)
{
    if(hasSelection()) {
        QPointF start(selectionAnchorHandle()->x(), selectionAnchorHandle()->y());
        QPointF end(selectionCursorHandle()->x(), selectionCursorHandle()->y());

        QPointF center = (start + end) / 2.0f;

        emit selected(center);
    }
}

void WordsCanvas::updateFromHandles()
{
    TextSelection::updateFromHandles();
}

QPointF WordsCanvas::documentToView(const QPointF& point)
{
    return d->canvas->viewMode()->documentToView(point, d->canvas->viewConverter());
}

QPointF WordsCanvas::viewToDocument(const QPointF& point)
{
    return d->canvas->viewMode()->viewToDocument(point, d->canvas->viewConverter());
}

void WordsCanvas::Private::updatePanGesture(const QPointF& location)
{
    q->updatePosition(TextSelection::UpdatePosition, location);
}

void WordsCanvas::Private::documentOffsetMoved(QPoint newOffset)
{
    q->updateHandlePositions();
    
    int pageSize = q->documentSize().height() / doc->pageCount();
    if(pageSize > 0) {
        int page = (newOffset.y() + pageSize/2) / pageSize;
        if(page > 0 && page < doc->pageCount()) {
            emit q->pageChanged(page);
        }
    }
}

#include "WordsCanvas.moc"
