#include "TablesCanvas.h"

#include <QtCore/QBuffer>
#include <QtCore/QTimer>
#include <QtGui/QGraphicsWidget>
#include <QtGui/QApplication>
#include <QtGui/QGraphicsSceneMouseEvent>

#include <KoView.h>
#include <KoToolManager.h>
#include <KoZoomController.h>
#include <KoDocumentInfo.h>
#include <KoToolProxy.h>
#include <KoToolManager.h>
#include <KoToolRegistry.h>

#include <tables/ui/Selection.h>
#include <tables/part/Doc.h>
#include <tables/part/CanvasItem.h>
#include <tables/Map.h>
#include <tables/Find.h>
#include <tables/part/ToolRegistry.h>
#include <tables/Sheet.h>
#include <tables/CellStorage.h>

#include "shared/ProgressProxy.h"

class TablesCanvas::Private
{
public:
    Private( TablesCanvas* qq)
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
    void documentOffsetMoved(const QPoint &newOffset);
    void updateSelectionFromHandles();
    void updateSelectionHandles();

    QString cellAsText(const Calligra::Tables::Cell &cell, bool addTab);

    TablesCanvas* q;

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

TablesCanvas::TablesCanvas(QDeclarativeItem* parent)
    : CanvasControllerDeclarative(parent), d(new Private(this))
{
    ProcessInputInterface::setupConnections(inputProxy(), this);
    connect(inputProxy(), SIGNAL(updatePanGesture(QPointF)), this, SLOT(updatePanGesture(QPointF)));

    KoZoomMode::setMinimumZoom(0.5);
    KoZoomMode::setMaximumZoom(2.0);

    connect(proxyObject, SIGNAL(moveDocumentOffset(QPoint)), this, SLOT(documentOffsetMoved(QPoint)));
}

TablesCanvas::~TablesCanvas()
{
    delete d;
}

int TablesCanvas::activeSheetIndex() const
{
    return d->activeSheetIndex;
}

QObject* TablesCanvas::doc() const
{
    return d->doc;
}

void TablesCanvas::setActiveSheetIndex(int index)
{
    d->activeSheetIndex = index;
    emit sheetChanged(d->activeSheetIndex);
    d->updateCanvas();
}

bool TablesCanvas::hasNextSheet() const
{
    return d->hasNextSheet;
}

void TablesCanvas::nextSheet()
{
    d->activeSheetIndex++;
    if(d->activeSheetIndex >= d->doc->map()->count()) {
        d->activeSheetIndex = 0;
    }
    emit sheetChanged(d->activeSheetIndex);
    d->updateCanvas();
}

bool TablesCanvas::hasPreviousSheet() const
{
    return d->hasPreviousSheet;
}

void TablesCanvas::previousSheet()
{
    d->activeSheetIndex--;
    if(d->activeSheetIndex < 0) {
        d->activeSheetIndex = d->doc->map()->count() - 1;
    }
    emit sheetChanged(d->activeSheetIndex);
    d->updateCanvas();
}

void TablesCanvas::changeSheet(int newIndex)
{
    d->canvas->setActiveSheet( d->doc->map()->sheet(newIndex) );
    emit sheetChanged(newIndex);
}

QString TablesCanvas::sheetName() const
{
    return d->canvas->activeSheet()-> sheetName();
}

int TablesCanvas::matchCount()
{
    return d->finder->matches().count();
}

void TablesCanvas::copySelection()
{
    Calligra::Tables::Selection *sel = d->canvas->selection();
    if (sel->isSingular()) {
        const Calligra::Tables::Cell cell(sel->firstSheet(), sel->firstRange().topLeft());
        QApplication::clipboard()->setText(cell.displayText().replace('\n', ' '));
    }
    else
    {
        QList<Calligra::Tables::Cell> copiedCells;
        QRect lastRange = sel->lastRange();
        // Find area
        int top = lastRange.top();
        int bottom = lastRange.bottom();
        int left = lastRange.left();
        int right = lastRange.right();

        QString result;
        for (int row = top; row <= bottom; ++row) {
            for (int col = left; col <= right; ++col) {
                Calligra::Tables::Cell cell(sel->lastSheet(), col, row);
                if(!copiedCells.contains(cell)) {
                    result += d->cellAsText(cell, col != right);
                    copiedCells.append(cell);
                }
            }
            result += '\n';
        }
        qDebug() << result;
        QApplication::clipboard()->setText(result);
    }
}

void TablesCanvas::loadDocument()
{
    emit progress(1);

    setCanvasMode(KoCanvasController::Infinite);

    Calligra::Tables::ToolRegistry::instance()->loadTools();
    Calligra::Tables::Doc* doc = new Calligra::Tables::Doc();
    d->doc = doc;
    d->updateCanvas();
    d->finder = new Calligra::Tables::Find(this);
    connect(d->finder, SIGNAL(matchFound(KoFindMatch)), SLOT(matchFound(KoFindMatch)));

    ProgressProxy *proxy = new ProgressProxy(this);
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

void TablesCanvas::find ( const QString& pattern )
{
    d->matchNumber = 0;
    d->finder->find(pattern);
}

void TablesCanvas::findFinished()
{
    d->finder->finished();
}

void TablesCanvas::findNext()
{
    d->finder->findNext();
}

void TablesCanvas::findPrevious()
{
    d->finder->findPrevious();
}

void TablesCanvas::setSelectionAnchorHandle ( QDeclarativeItem* handle )
{
    CanvasControllerDeclarative::setSelectionAnchorHandle(handle);
    connect(handle, SIGNAL(xChanged()), SLOT(updateSelectionFromHandles()));
    connect(handle, SIGNAL(yChanged()), SLOT(updateSelectionFromHandles()));
}

void TablesCanvas::setSelectionCursorHandle ( QDeclarativeItem* handle )
{
    CanvasControllerDeclarative::setSelectionCursorHandle(handle);
    connect(handle, SIGNAL(xChanged()), SLOT(updateSelectionFromHandles()));
    connect(handle, SIGNAL(yChanged()), SLOT(updateSelectionFromHandles()));
}

void TablesCanvas::Private::updateDocumentSize(const QSize& size)
{
    q->zoomController()->setDocumentSize(size);
}

void TablesCanvas::Private::updateCanvas()
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

void TablesCanvas::Private::matchFound ( KoFindMatch match )
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

void TablesCanvas::onSingleTap ( const QPointF& location )
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

void TablesCanvas::onDoubleTap ( const QPointF& location )
{

}

void TablesCanvas::onLongTap ( const QPointF& location )
{
    KoToolManager::instance()->switchToolRequested(d->KSpreadCellToolId);
    
    d->selection = d->canvas->activeSheet()->documentToCellCoordinates(QRectF(d->canvas->viewConverter()->viewToDocument(location), QSizeF(0, 0)));
    d->canvas->selection()->initialize(d->selection, d->canvas->activeSheet());
    d->canvas->updateCanvas(QRectF(x(), y(), width(), height()));

    d->updateSelectionHandles();
}

void TablesCanvas::onLongTapEnd ( const QPointF& location )
{
    QPointF start(selectionAnchorHandle()->x(), selectionAnchorHandle()->y());
    QPointF end(selectionCursorHandle()->x(), selectionCursorHandle()->y());

    QPointF center = (start + end) / 2.0f;

    emit selected(center);
}

void TablesCanvas::Private::updatePanGesture ( const QPointF& location )
{
    q->selectionCursorHandle()->setPos(location);
}

void TablesCanvas::Private::updateSelectionFromHandles()
{
    if(!q->selectionAnchorHandle()->isVisible() || !q->selectionCursorHandle()->isVisible()) {
        return;
    }

    QPointF docAnchorPos = canvas->viewConverter()->viewToDocument(QPointF(q->selectionAnchorHandle()->x(), q->selectionAnchorHandle()->y()) + q->documentOffset());
    QPointF docCursorPos = canvas->viewConverter()->viewToDocument(QPointF(q->selectionCursorHandle()->x(), q->selectionCursorHandle()->y()) + q->documentOffset());
    
    selection = canvas->activeSheet()->documentToCellCoordinates(QRectF(docAnchorPos, docCursorPos));

    canvas->selection()->initialize(selection, canvas->activeSheet());
    canvas->updateCanvas(QRectF(q->x(), q->y(), q->width(), q->height()));

    updateSelectionHandles();
}

void TablesCanvas::Private::updateSelectionHandles()
{
    if(!q->selectionAnchorHandle() || !q->selectionCursorHandle()) {
        return;
    }

    QRectF viewSelection = canvas->viewConverter()->documentToView(canvas->activeSheet()->cellCoordinatesToDocument(selection));
    
    q->selectionAnchorHandle()->blockSignals(true);
    q->selectionAnchorHandle()->setPos(viewSelection.topLeft() - q->documentOffset());
    q->selectionAnchorHandle()->blockSignals(false);
    q->selectionAnchorHandle()->setVisible(true);
    
    q->selectionCursorHandle()->blockSignals(true);
    q->selectionCursorHandle()->setPos(viewSelection.bottomRight() - q->documentOffset());
    q->selectionCursorHandle()->blockSignals(false);
    q->selectionCursorHandle()->setVisible(true);
}

void TablesCanvas::Private::documentOffsetMoved ( const QPoint& newOffset )
{
    if(!q->selectionAnchorHandle() || !q->selectionCursorHandle()) {
        return;
    }

    if(q->selectionAnchorHandle()->isVisible() && q->selectionCursorHandle()->isVisible()) {
        updateSelectionHandles();
    }
}

QString TablesCanvas::Private::cellAsText ( const Calligra::Tables::Cell& cell, bool addTab )
{
    QString result;
    if (!cell.isDefault()) {
        result += cell.displayText().replace('\n', ' ');
    }
    if (addTab) {
        result += '\t';
    }
    return result;
}

#include "TablesCanvas.moc"
