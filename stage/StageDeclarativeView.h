#ifndef CMSTAGEDECLARATIVEVIEW_H
#define CMSTAGEDECLARATIVEVIEW_H

#include <QtCore/QObject>
#include <KoPAViewBase.h>

class KoPACanvasItem;

class StageDeclarativeView : public QObject, public KoPAViewBase
{
    Q_OBJECT
public:
    explicit StageDeclarativeView(KoZoomController* zoomController,
                                    KoPADocument* doc,
                                    KoPACanvasItem* canvas);
    virtual ~StageDeclarativeView();

    virtual KoPADocument* kopaDocument() const;
    virtual KoPACanvasBase* kopaCanvas() const;
    virtual KoZoomController* zoomController() const;
    virtual void doUpdateActivePage(KoPAPageBase* page);
    virtual void setActivePage(KoPAPageBase* page);
    virtual KoPAPageBase* activePage() const;
    virtual void navigatePage(KoPageApp::PageNavigation pageNavigation);
    virtual void setActionEnabled(int actions, bool enable);
    virtual void updatePageNavigationActions();
    virtual void insertPage();
    virtual void pagePaste();
    virtual void editPaste();
    virtual void setShowRulers(bool show);

    Q_SLOT void setPage(int newPage);
    int page() const;

Q_SIGNALS:
    void updateSize(const QSize&);

private slots:

    void previousPage();
    void nextPage();
    void firstPage();
    void lastPage();

private:
    class Private;
    Private * const d;
};

#endif // CMSTAGEDECLARATIVEVIEW_H
