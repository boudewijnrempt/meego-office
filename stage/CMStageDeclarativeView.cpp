#include "CMStageDeclarativeView.h"
#include <KoPAPageBase.h>
#include <part/KPrViewModePresentation.h>
#include <KoPAViewModeNormal.h>
#include <KoPAPage.h>
#include <KoPADocument.h>
#include <KoPACanvasBase.h>
#include <KoZoomController.h>
#include <KoShapeManager.h>
#include <KoPACanvasItem.h>
#include <kactioncollection.h>
#include <KoSelection.h>
#include <KoPAMasterPage.h>
#include <KoShapeLayer.h>

class CMStageDeclarativeView::Private
{
public:
    Private()
        : page(0)
        , zoomController(0)
        , doc(0)
        , canvas(0)
        , viewMode(0)
        , actionCollection(0)
    { }
    ~Private() { }

    KoPAPageBase* page;

    KoZoomController* zoomController;
    KoPADocument* doc;
    KoPACanvasItem* canvas;
    KoPAViewModeNormal* viewMode;

    KActionCollection *actionCollection;
};

CMStageDeclarativeView::CMStageDeclarativeView(KoZoomController* zoomController,
                                               KoPADocument* doc,
                                               KoPACanvasItem* canvas)
    : QObject(0)
    , d(new Private)
{
    d->zoomController = zoomController;
    d->doc = doc;

    d->canvas = canvas;
    Q_ASSERT(d->canvas);
    Q_ASSERT(d->canvas->resourceManager());

    d->canvas->setAttribute(Qt::WA_OpaquePaintEvent, true);
    d->canvas->setAutoFillBackground(false);

    d->canvas->setView(this);
    d->actionCollection = new KActionCollection(this);

    d->actionCollection->addAction(KStandardAction::Prior, "page_previous", this, SLOT(previousPage()));
    d->actionCollection->addAction(KStandardAction::Next, "page_next", this, SLOT(nextPage()));
    d->actionCollection->addAction(KStandardAction::FirstPage, "page_first", this, SLOT(firstPage()));
    d->actionCollection->addAction(KStandardAction::LastPage, "page_last", this, SLOT(lastPage()));

    d->viewMode = new KoPAViewModeNormal(this, d->canvas);
    setViewMode(d->viewMode);

    if(doc->pageCount() > 0) {
        doUpdateActivePage(doc->pageByIndex(0, false));
    }
}

CMStageDeclarativeView::~CMStageDeclarativeView()
{
    delete d;
}

KoPADocument* CMStageDeclarativeView::kopaDocument() const
{
    return d->doc;
}

KoZoomController* CMStageDeclarativeView::zoomController() const
{
    return d->zoomController;
}

void CMStageDeclarativeView::doUpdateActivePage(KoPAPageBase* page)
{
    bool activePageChanged = page != d->page;

    setActivePage(page);

    d->canvas->updateSize();
    KoPageLayout &layout = page->pageLayout();

    QSizeF pageSize(layout.width, layout.height);

    d->canvas->setDocumentOrigin(QPointF(0,0));
    // the page is in the center of the canvas
    d->zoomController->setDocumentSize(pageSize);
    d->zoomController->setPageSize(pageSize);

    d->canvas->resourceManager()->setResource(KoCanvasResource::PageSize, pageSize);
    d->canvas->update();

    updatePageNavigationActions();

    if (activePageChanged) {
        proxyObject->emitActivePageChanged();
    }
    d->canvas->updateSize();
    emit updateSize(pageSize.toSize());
}



void CMStageDeclarativeView::setShowRulers(bool show)
{
    Q_UNUSED(show);
}

void CMStageDeclarativeView::editPaste()
{
}

void CMStageDeclarativeView::pagePaste()
{
}

void CMStageDeclarativeView::insertPage()
{
}

void CMStageDeclarativeView::updatePageNavigationActions()
{
    KoPADocument *doc = qobject_cast<KoPADocument*>(d->doc);
    int index = doc->pageIndex(activePage());
    int pageCount = doc->pages(viewMode()->masterMode()).count();

    d->actionCollection->action("page_previous")->setEnabled(index > 0);
    d->actionCollection->action("page_first")->setEnabled(index > 0);
    d->actionCollection->action("page_next")->setEnabled(index < pageCount - 1);
    d->actionCollection->action("page_last")->setEnabled(index < pageCount - 1);
}

void CMStageDeclarativeView::setActionEnabled(int actions, bool enable)
{
    Q_UNUSED(actions)
    Q_UNUSED(enable)
}

void CMStageDeclarativeView::navigatePage(KoPageApp::PageNavigation pageNavigation)
{
    KoPADocument *doc = qobject_cast<KoPADocument*>(d->doc);
    KoPAPageBase *newPage = doc->pageByNavigation(d->page, pageNavigation);

    if (newPage != d->page) {
        proxyObject->updateActivePage(newPage);
    }
}

KoPAPageBase* CMStageDeclarativeView::activePage() const
{
    return d->page;
}

void CMStageDeclarativeView::setActivePage(KoPAPageBase* page)
{
    if (!page) {
        return;
    }
    if (d->page) {
        d->canvas->shapeManager()->removeAdditional(d->page);
    }

    d->page = page;

    d->canvas->shapeManager()->addAdditional(d->page);

    QList<KoShape*> shapes = page->shapes();
    d->canvas->shapeManager()->setShapes(shapes, KoShapeManager::AddWithoutRepaint);

    // Make the top most layer active
    if(!shapes.isEmpty()) {
        KoShapeLayer* layer = dynamic_cast<KoShapeLayer*>(shapes.last());
        d->canvas->shapeManager()->selection()->setActiveLayer(layer);
    }

    // if the page is not a master page itself, set shapes of the master page
    KoPAPage * paPage = dynamic_cast<KoPAPage *>(page);

    if (paPage) {
        KoPAMasterPage *masterPage = paPage->masterPage();
        QList<KoShape*> masterShapes = masterPage->shapes();
        d->canvas->masterShapeManager()->setShapes(masterShapes,
                                                      KoShapeManager::AddWithoutRepaint);
        // Make the top most layer active
        if (!masterShapes.isEmpty()) {
            KoShapeLayer* layer = dynamic_cast<KoShapeLayer*>(masterShapes.last());
            d->canvas->masterShapeManager()->selection()->setActiveLayer(layer);
        }
    }
    else {
        // if the page is a master page no shapes are in the masterShapeManager
        d->canvas->masterShapeManager()->setShapes(QList<KoShape*>());
    }

    // Set the current page number in the canvas resource provider
    KoPADocument *doc = qobject_cast<KoPADocument*>(d->doc);
    if (doc) {
        d->canvas->resourceManager()->setResource(KoCanvasResource::CurrentPage,
                                                     doc->pageIndex(d->page) + 1);
    }

}



KoPACanvasBase* CMStageDeclarativeView::kopaCanvas() const
{
    return d->canvas;
}


void CMStageDeclarativeView::previousPage()
{
    navigatePage(KoPageApp::PagePrevious);
}

void CMStageDeclarativeView::nextPage()
{
    navigatePage(KoPageApp::PageNext);
}

void CMStageDeclarativeView::firstPage()
{
    navigatePage(KoPageApp::PageFirst);
}

void CMStageDeclarativeView::lastPage()
{
    navigatePage(KoPageApp::PageLast);
}

void CMStageDeclarativeView::setPage(int newPage)
{
    KoPADocument *doc = qobject_cast<KoPADocument*>(d->doc);
    KoPAPageBase *theNewPage = doc->pageByIndex(newPage, false);// pageByNavigation(d->page, pageNavigation);
    
    if (theNewPage != d->page) {
        proxyObject->updateActivePage(theNewPage);
    }
}
