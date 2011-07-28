#ifndef PDFCANVASCONTROLLER_H
#define PDFCANVASCONTROLLER_H

#include "shared/CanvasControllerDeclarative.h"
#include "shared/ProcessInputInterface.h"
#include "shared/SearchingInterface.h"

class PDFDocument;
class PDFCanvasController : public CanvasControllerDeclarative, public ProcessInputInterface, public SearchingInterface
{
    Q_OBJECT
    Q_PROPERTY(QObject* document READ document)
    Q_PROPERTY(int page READ page WRITE setPage)
    Q_PROPERTY(int pageCount READ pageCount)
    
public:
    PDFCanvasController ( QDeclarativeItem* parent = 0 );
    virtual ~PDFCanvasController();

    Q_INVOKABLE void copySelection();

    PDFDocument *document() const;
    int pageCount() const;
    int page() const;

    Q_INVOKABLE virtual int matchCount();

public Q_SLOTS:
    void loadDocument();
    void setPage(int newPage);
    void goToNextPage();
    void goToPreviousPage();

    virtual void setSelectionAnchorHandle ( QDeclarativeItem* handle );
    virtual void setSelectionCursorHandle ( QDeclarativeItem* handle );

    virtual void find ( const QString& pattern );
    virtual void findNext();
    virtual void findPrevious();
    virtual void findFinished();

    virtual void onLongTapEnd ( const QPointF& location = QPointF() );

Q_SIGNALS:
    void selected(const QPointF &origin);
    void pageChanged(int newPage);

    void findMatchFound ( int match );

private:
    class Private;
    const QScopedPointer<Private> d;

    Q_PRIVATE_SLOT(d, void updatePanGesture(const QPointF &location));
    Q_PRIVATE_SLOT(d, void moveDocumentOffset(const QPoint &offset));
    Q_PRIVATE_SLOT(d, void documentLoaded());
    Q_PRIVATE_SLOT(d, void searchUpdate());
    Q_PRIVATE_SLOT(d, void matchFound(const KoFindMatch &match));
    Q_PRIVATE_SLOT(d, void updateDocSize(const QSizeF &newSize));
    Q_PRIVATE_SLOT(d, void updateSelectionFromHandles());

private Q_SLOTS:
    virtual void onSingleTap ( const QPointF& location );
    virtual void onLongTap ( const QPointF& location );
    virtual void onDoubleTap ( const QPointF& location );
    
};

#endif // PDFCANVASCONTROLLER_H
