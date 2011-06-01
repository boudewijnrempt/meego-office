#ifndef CALLIGRAMOBILE_TABLESCANVAS_H
#define CALLIGRAMOBILE_TABLESCANVAS_H

#include "../CMCanvasControllerDeclarative.h"
#include "../CMSearchingInterface.h"
#include "../CMProcessInputInterface.h"

#include <KoFindMatch.h>

class CMTablesCanvas : public CMCanvasControllerDeclarative, public CMSearchingInterface, public CMProcessInputInterface
{
    Q_OBJECT
    Q_PROPERTY(int activeSheetIndex READ activeSheetIndex WRITE setActiveSheetIndex)
    Q_PROPERTY(QObject* document READ doc)
    Q_PROPERTY(bool hasNextSheet READ hasNextSheet NOTIFY hasNextSheetChanged)
    Q_PROPERTY(bool hasPreviousSheet READ hasNextSheet NOTIFY hasPreviousSheetChanged)
    Q_PROPERTY(int sheet READ activeSheetIndex WRITE changeSheet NOTIFY sheetChanged)
    Q_PROPERTY(QString sheetName READ sheetName)

public:
    explicit CMTablesCanvas(QDeclarativeItem* parent = 0);
    virtual ~CMTablesCanvas();

    int activeSheetIndex() const;
    void setActiveSheetIndex(int index);

    bool hasNextSheet() const;
    bool hasPreviousSheet() const;

    QString sheetName() const;

    QObject* doc() const;

    Q_INVOKABLE int matchCount();

    Q_INVOKABLE void copySelection();

public Q_SLOTS:
    void nextSheet();
    void previousSheet();
    void changeSheet(int newIndex);
    void loadDocument();

    void find(const QString& pattern);
    void findPrevious();
    void findNext();
    void findFinished();

    virtual void onLongTapEnd( const QPointF &location = QPointF() );

    virtual void setSelectionAnchorHandle(QDeclarativeItem* handle);
    virtual void setSelectionCursorHandle(QDeclarativeItem* handle);

signals:
    void hasNextSheetChanged();
    void hasPreviousSheetChanged();
    void sheetChanged(int newIndex);
    void findMatchFound(int match);
    void selected(const QPointF &origin);

protected:
    void handleShortTap(QPointF pos);

private:
    class Private;
    Private * const d;
    
    Q_PRIVATE_SLOT(d, void updateDocumentSize(const QSize& size));
    Q_PRIVATE_SLOT(d, void matchFound(KoFindMatch match));
    Q_PRIVATE_SLOT(d, void updatePanGesture(const QPointF &location));
    Q_PRIVATE_SLOT(d, void updateSelectionFromHandles());
    Q_PRIVATE_SLOT(d, void documentOffsetMoved(const QPoint &newOffset));

private Q_SLOTS:
    virtual void onSingleTap(const QPointF &location);
    virtual void onDoubleTap(const QPointF &location);
    virtual void onLongTap(const QPointF &location);
    
};

#endif // CALLIGRAMOBILE_TABLESCANVAS_H
