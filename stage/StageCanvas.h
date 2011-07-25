#ifndef CALLIGRAMOBILE_STAGECANVAS_H
#define CALLIGRAMOBILE_STAGECANVAS_H

#include "shared/CanvasControllerDeclarative.h"
#include "shared/SearchingInterface.h"
#include "shared/ProcessInputInterface.h"
#include "shared/TextSelection.h"

class KoFindMatch;
class StageCanvas : public CanvasControllerDeclarative,
                      public SearchingInterface,
                      private ProcessInputInterface,
                      private TextSelection
{
    Q_OBJECT
    Q_PROPERTY(QObject* document READ doc)
    Q_PROPERTY(QObject* view READ view)
    Q_PROPERTY(int slide READ slide WRITE changeSlide NOTIFY slideChanged)
    Q_PROPERTY(int slideCount READ slideCount)
    Q_PROPERTY(QString currentPageNotes READ currentPageNotes NOTIFY currentPageNotesChanged)

public:
    StageCanvas(QDeclarativeItem* parent = 0);
    virtual ~StageCanvas();

    QObject* doc() const;
    QObject* view() const;

    int slide() const;
    int slideCount() const;

    Q_INVOKABLE virtual int matchCount();

    Q_INVOKABLE void copySelection();

public Q_SLOTS:
    void loadDocument();
    void changeSlide(int newSlide);

    virtual void find ( const QString& pattern );
    virtual void findFinished();
    virtual void findNext();
    virtual void findPrevious();
    
    virtual bool canHavePageNotes() const { return true; }
    virtual QString currentPageNotes() const;

    virtual void setSelectionAnchorHandle(QDeclarativeItem* handle);
    virtual void setSelectionCursorHandle(QDeclarativeItem* handle);

    virtual void onLongTapEnd(const QPointF& location = QPointF());

Q_SIGNALS:
    void slideChanged(int newSlide);
    void findMatchFound ( int match );
    void selected(const QPointF &origin);
    void currentPageNotesChanged();

private:
    class Private;
    Private * const d;

    Q_PRIVATE_SLOT(d, void setDocumentSize(const QSize& size));
    Q_PRIVATE_SLOT(d, void matchFound(KoFindMatch match));
    Q_PRIVATE_SLOT(d, void update());
    Q_PRIVATE_SLOT(d, void updatePanGesture(const QPointF &location));
    Q_PRIVATE_SLOT(d, void moveSelectionHandles());

    virtual QPointF documentToView(const QPointF& point);
    virtual QPointF viewToDocument(const QPointF& point);

private Q_SLOTS:
    virtual void updateFromHandles();
    virtual void onSingleTap(const QPointF &location);
    virtual void onDoubleTap ( const QPointF& location );
    virtual void onLongTap ( const QPointF& location );
};

#endif // CALLIGRAMOBILE_STAGECANVAS_H
