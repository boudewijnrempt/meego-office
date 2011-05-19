#ifndef CALLIGRAMOBILE_WORDSCANVAS_H
#define CALLIGRAMOBILE_WORDSCANVAS_H

#include "CMCanvasControllerDeclarative.h"
#include "CMSearchingInterface.h"
#include "CMProcessInputInterface.h"
#include "CMTextSelection.h"

class CMWordsCanvas : public CMCanvasControllerDeclarative,
                      public CMSearchingInterface,
                      private CMProcessInputInterface,
                      private CMTextSelection
{
    Q_OBJECT
    Q_PROPERTY(QObject* document READ doc)
    Q_PROPERTY(int page READ page WRITE changePage NOTIFY pageChanged)
    Q_PROPERTY(int pageCount READ pageCount)

public:
    CMWordsCanvas(QDeclarativeItem* parent = 0);
    virtual ~CMWordsCanvas();

    QObject* doc() const;
    int page() const;
    int pageCount() const;

    Q_INVOKABLE int matchCount();

public Q_SLOTS:
    void loadDocument();

    void changePage(int newPage);
    bool hasNextPage() const;
    bool hasPreviousPage() const;
    void goToNextPage();
    void goToPreviousPage();

    void find(const QString& pattern);
    void findPrevious();
    void findNext();
    void findFinished();

    virtual void setSelectionAnchorHandle(QDeclarativeItem* handle);
    virtual void setSelectionCursorHandle(QDeclarativeItem* handle);

Q_SIGNALS:
    void pageChanged(int newPage);
    void findMatchFound(int match);
    void doubleTapped();
    void selected(const QPointF &origin);

private:
    class Private;
    Private * const d;

    Q_PRIVATE_SLOT(d, void matchFound(KoFindMatch));
    Q_PRIVATE_SLOT(d, void update());
    Q_PRIVATE_SLOT(d, void updatePanGesture(const QPointF &location));

    QPointF documentToView(const QPointF &point);
    QPointF viewToDocument(const QPointF &point);

private Q_SLOTS:
    virtual void updateFromHandles();
    virtual void onSingleTap(const QPointF &location);
    virtual void onDoubleTap ( const QPointF& location );
    virtual void onLongTap ( const QPointF& location );
    virtual void onLongTapEnd( const QPointF &location );
};

#endif // CALLIGRAMOBILE_WORDSCANVAS_H
