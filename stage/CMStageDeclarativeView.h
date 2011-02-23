#ifndef CMSTAGEDECLARATIVEVIEW_H
#define CMSTAGEDECLARATIVEVIEW_H

#include <KoPAViewBase.h>


class CMStageDeclarativeView : public KoPAViewBase
{
public:
    explicit CMStageDeclarativeView(KoZoomController* zoomController, KoPADocument* doc, KoPACanvasBase* canvas);
    virtual ~CMStageDeclarativeView();
    
    virtual void setShowRulers(bool show);
    virtual void editPaste();
    virtual void pagePaste();
    virtual void insertPage();
    virtual void updatePageNavigationActions();
    virtual void setActionEnabled(int actions, bool enable);
    virtual void navigatePage(KoPageApp::PageNavigation pageNavigation);
    virtual KoPAPageBase* activePage() const;
    virtual void setActivePage(KoPAPageBase* page);
    virtual void doUpdateActivePage(KoPAPageBase* page);
    virtual KoZoomController* zoomController() const;
    virtual KoPADocument* kopaDocument() const;
    virtual KoPACanvasBase* kopaCanvas() const;

private:
    class Private;
    Private * const d;
};

#endif // CMSTAGEDECLARATIVEVIEW_H
