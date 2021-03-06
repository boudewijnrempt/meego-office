#ifndef CALLIGRAMOBILE_WORDSCANVAS_H
#define CALLIGRAMOBILE_WORDSCANVAS_H

#include "shared/CanvasControllerDeclarative.h"
#include "shared/SearchingInterface.h"
#include "shared/ProcessInputInterface.h"
#include "shared/TextSelection.h"

class WordsCanvas : public CanvasControllerDeclarative,
                    public SearchingInterface,
                    private ProcessInputInterface,
                    private TextSelection
{
    Q_OBJECT
    Q_PROPERTY(QObject* document READ doc)
    Q_PROPERTY(int page READ page WRITE changePage NOTIFY pageChanged)
    Q_PROPERTY(int pageCount READ pageCount)

public:
    WordsCanvas(QDeclarativeItem* parent = 0);
    virtual ~WordsCanvas();

    QObject* doc() const;
    int page() const;
    int pageCount() const;

    Q_INVOKABLE int matchCount();

    Q_INVOKABLE void copySelection();

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

    virtual void onLongTapEnd( const QPointF &location = QPointF() );

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
    Q_PRIVATE_SLOT(d, void documentOffsetMoved(QPoint newOffset));

    QPointF documentToView(const QPointF &point);
    QPointF viewToDocument(const QPointF &point);

private Q_SLOTS:
    virtual void updateFromHandles();
    virtual void onSingleTap(const QPointF &location);
    virtual void onDoubleTap ( const QPointF& location );
    virtual void onLongTap ( const QPointF& location );
};

#endif // CALLIGRAMOBILE_WORDSCANVAS_H
