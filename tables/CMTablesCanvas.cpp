#include "CMTablesCanvas.h"

#include <QtGui/QGraphicsWidget>
#include <QtCore/QTimer>
#include <QtGui/QGraphicsSceneMouseEvent>

#include <tables/ui/Selection.h>
#include <tables/part/Doc.h>
#include <tables/part/CanvasItem.h>
#include <tables/Map.h>
#include <tables/Find.h>
#include <tables/part/ToolRegistry.h>
#include <KoView.h>
#include <KoToolManager.h>
#include <KoZoomController.h>
#include <KoDocumentInfo.h>
#include <KoToolProxy.h>
#include <KoToolManager.h>

#include "CMProgressProxy.h"
#include <Sheet.h>
#include <KoToolRegistry.h>
#include <../../KDE/calligra-master/tables/CellStorage.h>

class CMTablesCanvas::Private
{
public:
    Private(CMTablesCanvas* qq)
        : q(qq),
          activeSheetIndex(-1),
          doc(0),
          canvas(0),
          KSpreadCellToolId("KSpreadCellToolId")
    {}

    void updateCanvas();
    void updateDocumentSize(const QSize &size);
    void matchFound(KoFindMatch match);
    void updatePanGesture(const QPointF &location);
    void updateSelectionFromHandles();
    void updateSelectionHandles();

    CMTablesCanvas* q;

    int activeSheetIndex;
    Calligra::Tables::Doc* doc;
    Calligra::Tables::CanvasItem* canvas;
    Calligra::Tables::Find* finder;

    bool hasNextSheet;
    bool hasPreviousSheet;

    const QString KSpreadCellToolId;
    int matchNumber;

    QRect selection;
};

CMTablesCanvas::CMTablesCanvas(QDeclarativeItem* parent)
    : CMCanvasControllerDeclarative(parent), d(new Private(this))
{
    CMProcessInputInterface::setupConnections(inputProxy(), this);
    connect(inputProxy(), SIGNAL(updatePanGesture(QPointF)), this, SLOT(updatePanGesture(QPointF)));

    KoZoomMode::setMinimumZoom(0.5);
    KoZoomMode::setMaximumZoom(2.0);
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
    emit sheetChanged(d->activeSheetIndex);
    d->updateCanvas();
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
    emit sheetChanged(d->activeSheetIndex);
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
    emit sheetChanged(d->activeSheetIndex);
    d->updateCanvas();
}

void CMTablesCanvas::changeSheet(int newIndex)
{
    d->canvas->setActiveSheet( d->doc->map()->sheet(newIndex) );
    emit sheetChanged(newIndex);
}

QString CMTablesCanvas::sheetName() const
{
    return d->canvas->activeSheet()-> sheetName();
}

int CMTablesCanvas::matchCount()
{
    return d->finder->matches().count();
}

void CMTablesCanvas::loadDocument()
{
    emit progress(1);

    setCanvasMode(KoCanvasController::Infinite);

    Calligra::Tables::ToolRegistry::instance()->loadTools();
    Calligra::Tables::Doc* doc = new Calligra::Tables::Doc();
    d->doc = doc;
    d->updateCanvas();
    d->finder = new Calligra::Tables::Find(this);
    connect(d->finder, SIGNAL(matchFound(KoFindMatch)), SLOT(matchFound(KoFindMatch)));

    CMProgressProxy *proxy = new CMProgressProxy(this);
    doc->setProgressProxy(proxy);

    connect(proxy, SIGNAL(valueChanged(int)), SIGNAL(progress(int)));

    if(!d->doc->openUrl(KUrl(file()))) {
        kWarning() << "Could not open file:" << file();
        return;
    }

    d->activeSheetIndex = 0;
    d->updateCanvas();

    emit progress(100);
    emit completed();
    emit sheetChanged(0);
}

void CMTablesCanvas::find ( const QString& pattern )
{
    d->matchNumber = 0;
    d->finder->find(pattern);
}

void CMTablesCanvas::findFinished()
{
    d->finder->finished();
}

void CMTablesCanvas::findNext()
{
    d->finder->findNext();
}

void CMTablesCanvas::findPrevious()
{
    d->finder->findPrevious();
}

void CMTablesCanvas::setSelectionAnchorHandle ( QDeclarativeItem* handle )
{
    CMCanvasControllerDeclarative::setSelectionAnchorHandle(handle);
    connect(handle, SIGNAL(xChanged()), SLOT(updateSelectionFromHandles()));
    connect(handle, SIGNAL(yChanged()), SLOT(updateSelectionFromHandles()));
}

void CMTablesCanvas::setSelectionCursorHandle ( QDeclarativeItem* handle )
{
    CMCanvasControllerDeclarative::setSelectionCursorHandle(handle);
    connect(handle, SIGNAL(xChanged()), SLOT(updateSelectionFromHandles()));
    connect(handle, SIGNAL(yChanged()), SLOT(updateSelectionFromHandles()));
}

void CMTablesCanvas::Private::updateDocumentSize(const QSize& size)
{
    q->zoomController()->setDocumentSize(size);
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
        connect(canvas, SIGNAL(documentSizeChanged(QSize)), q, SLOT(updateDocumentSize(QSize)));
    }

    canvas->updateCanvas(QRectF(0, 0, q->width(), q->height()));
    if (canvas && activeSheetIndex >= 0) {
        canvas->setActiveSheet(doc->map()->sheet(activeSheetIndex));
        finder->setCurrentSheet(doc->map()->sheet(activeSheetIndex));
    }
}

void CMTablesCanvas::Private::matchFound ( KoFindMatch match )
{
    KoToolManager::instance()->switchToolRequested(KSpreadCellToolId);

    matchNumber = finder->matches().indexOf(match) + 1;
    emit q->findMatchFound(matchNumber);

    Calligra::Tables::Sheet* sheet = match.container().value<Calligra::Tables::Sheet*>();
    Calligra::Tables::Cell cell = match.location().value<Calligra::Tables::Cell>();
    canvas->selection()->initialize(cell.cellPosition());
    QRectF pos = sheet->cellCoordinatesToDocument(QRect(canvas->selection()->anchor(), canvas->selection()->cursor()));
    pos = canvas->viewConverter()->documentToView(pos);
    q->ensureVisible(pos, false);
}

void CMTablesCanvas::onSingleTap ( const QPointF& location )
{
    d->canvas->selection()->clear();
    d->canvas->updateCanvas(QRectF(x(), y(), width(), height()));
    selectionAnchorHandle()->hide();
    selectionCursorHandle()->hide();
//     KoToolManager::instance()->switchToolRequested(d->KSpreadCellToolId);
// 
//     // convert the position from qgraphicsscene to the canvas item
//     QPointF docPos = canvas()->viewConverter()->viewToDocument(canvas()->canvasItem()->mapFromScene(location) + documentOffset());
//     
//     // Click...
//     QMouseEvent press(QEvent::MouseButtonPress,
//                       location.toPoint(),
//                       Qt::LeftButton,
//                       Qt::LeftButton,
//                       Qt::NoModifier);
//     canvas()->toolProxy()->mousePressEvent(&press, docPos);
// 
// 
//     // And release...
//     QMouseEvent release(QEvent::MouseButtonRelease,
//                         location.toPoint(),
//                         Qt::LeftButton,
//                         Qt::LeftButton,
//                         Qt::NoModifier);
//     canvas()->toolProxy()->mouseReleaseEvent(&release, docPos);
}

void CMTablesCanvas::onDoubleTap ( const QPointF& location )
{

}

void CMTablesCanvas::onLongTap ( const QPointF& location )
{
    KoToolManager::instance()->switchToolRequested(d->KSpreadCellToolId);
    
    d->selection = d->canvas->activeSheet()->documentToCellCoordinates(QRectF(d->canvas->viewConverter()->viewToDocument(location), QSizeF(0, 0)));
    d->canvas->selection()->initialize(d->selection, d->canvas->activeSheet());
    d->canvas->updateCanvas(QRectF(x(), y(), width(), height()));

    d->updateSelectionHandles();
}

void CMTablesCanvas::onLongTapEnd ( const QPointF& location )
{
    QPointF start(selectionAnchorHandle()->x(), selectionAnchorHandle()->y());
    QPointF end(selectionCursorHandle()->x(), selectionCursorHandle()->y());

    QPointF center = (start + end) / 2.0f;

    emit selected(center);
}

void CMTablesCanvas::Private::updatePanGesture ( const QPointF& location )
{
    QPoint diff = canvas->activeSheet()->documentToCellCoordinates(QRectF(canvas->viewConverter()->viewToDocument(location), QSizeF(0, 0))).topLeft();
    diff -= selection.topLeft();
    
    selection.setSize(QSize(diff.x(), diff.y()));

    canvas->selection()->initialize(selection, canvas->activeSheet());
    canvas->updateCanvas(QRectF(q->x(), q->y(), q->width(), q->height()));
    
    updateSelectionHandles();
}

void CMTablesCanvas::Private::updateSelectionFromHandles()
{
    if(!q->selectionAnchorHandle()->isVisible() || !q->selectionCursorHandle()->isVisible()) {
        return;
    }
    
    selection = canvas->activeSheet()->documentToCellCoordinates(QRectF(canvas->viewConverter()->viewToDocument(QPointF(q->selectionAnchorHandle()->x(), q->selectionAnchorHandle()->y())), canvas->viewConverter()->viewToDocument(QPointF(q->selectionCursorHandle()->x(), q->selectionCursorHandle()->y()))));

    canvas->selection()->initialize(selection, canvas->activeSheet());
    canvas->updateCanvas(QRectF(q->x(), q->y(), q->width(), q->height()));

    updateSelectionHandles();
}

void CMTablesCanvas::Private::updateSelectionHandles()
{
    if(!q->selectionAnchorHandle() || !q->selectionCursorHandle()) {
        return;
    }

    QRectF viewSelection = canvas->viewConverter()->documentToView(canvas->activeSheet()->cellCoordinatesToDocument(selection));
    
    q->selectionAnchorHandle()->blockSignals(true);
    q->selectionAnchorHandle()->setPos(viewSelection.topLeft());
    q->selectionAnchorHandle()->blockSignals(false);
    q->selectionAnchorHandle()->setVisible(true);
    
    q->selectionCursorHandle()->blockSignals(true);
    q->selectionCursorHandle()->setPos(viewSelection.bottomRight());
    q->selectionCursorHandle()->blockSignals(false);
    q->selectionCursorHandle()->setVisible(true);
}

#include "CMTablesCanvas.moc"
