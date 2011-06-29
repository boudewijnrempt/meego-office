#ifndef PDFCANVASCONTROLLER_H
#define PDFCANVASCONTROLLER_H

#include <CMCanvasControllerDeclarative.h>
#include <CMProcessInputInterface.h>


class PDFCanvasController : public CMCanvasControllerDeclarative, public CMProcessInputInterface
{
Q_OBJECT
public:
    PDFCanvasController ( QDeclarativeItem* parent = 0 );
    virtual ~PDFCanvasController();

    Q_INVOKABLE void copySelection();

public Q_SLOTS:
    void loadDocument();

Q_SIGNALS:
    void selected(const QPointF &origin);

private:
    class Private;
    const QScopedPointer<Private> d;

    Q_PRIVATE_SLOT(d, void updatePanGesture(const QPointF &location));
    Q_PRIVATE_SLOT(d, void moveDocumentOffset(const QPoint &offset));

private Q_SLOTS:
    virtual void onSingleTap ( const QPointF& location );
    virtual void onLongTap ( const QPointF& location );
    virtual void onLongTapEnd ( const QPointF& location );
    virtual void onDoubleTap ( const QPointF& location );
    
};

#endif // PDFCANVASCONTROLLER_H
