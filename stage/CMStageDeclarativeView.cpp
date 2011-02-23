#include "CMStageDeclarativeView.h"
#include <KoPAPageBase.h>
#include <part/KPrViewModePresentation.h>
#include <KoPAViewModeNormal.h>
#include <KoPAPage.h>
#include <KoPADocument.h>
#include <KoPACanvasBase.h>

class CMStageDeclarativeView::Private
{
public:
    Private()
        : page(0)
    { }
    ~Private() { }

    KoPAPageBase* page;

    KoZoomController* zoomController;
    KoPADocument* doc;
    KoPACanvasBase* canvas;
    KoPAViewModeNormal* viewMode;
};

CMStageDeclarativeView::CMStageDeclarativeView(KoZoomController* zoomController, KoPADocument* doc, KoPACanvasBase* canvas)
    : d(new Private)
{
    d->zoomController = zoomController;
    d->doc = doc;
    d->canvas = canvas;
    d->canvas->setView(this);

    d->viewMode = new KoPAViewModeNormal(this, canvas);
    setViewMode(d->viewMode);
}

CMStageDeclarativeView::~CMStageDeclarativeView()
{
    delete d;
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

}

void CMStageDeclarativeView::setActionEnabled(int actions, bool enable)
{
    Q_UNUSED(actions)
    Q_UNUSED(enable)
}

void CMStageDeclarativeView::navigatePage(KoPageApp::PageNavigation pageNavigation)
{
    Q_UNUSED(pageNavigation)
}

KoPAPageBase* CMStageDeclarativeView::activePage() const
{
    return d->page;
}

void CMStageDeclarativeView::setActivePage(KoPAPageBase* page)
{
    d->page = page;
}

void CMStageDeclarativeView::doUpdateActivePage(KoPAPageBase* page)
{
    d->page = page;
    d->page->update();
}

KoZoomController* CMStageDeclarativeView::zoomController() const
{
    return d->zoomController;
}

KoPADocument* CMStageDeclarativeView::kopaDocument() const
{
    return d->doc;
}

KoPACanvasBase* CMStageDeclarativeView::kopaCanvas() const
{
    return d->canvas;
}

