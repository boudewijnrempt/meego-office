#ifndef PDFCANVAS_H
#define PDFCANVAS_H

#include <QtGui/QGraphicsWidget>
#include <KoCanvasBase.h>
#include <KoShapeControllerBase.h>

class PDFDocument;
class PDFCanvas : public QGraphicsWidget, public KoCanvasBase, public KoShapeControllerBase
{
Q_OBJECT
public:
    PDFCanvas(PDFDocument *document, QGraphicsItem *parentItem = 0);
    virtual ~PDFCanvas();

    virtual void addShape ( KoShape* shape );
    virtual void removeShape ( KoShape* shape );

    virtual KoShapeManager* shapeManager() const;

    QWidget *canvasWidget();
    const QWidget *canvasWidget() const;

    virtual QGraphicsWidget* canvasItem();
    virtual const QGraphicsWidget* canvasItem() const;

    virtual void updateInputMethodInfo();
    virtual KoUnit unit() const;

    virtual KoViewConverter* viewConverter() const;
    virtual KoToolProxy* toolProxy() const;
    virtual void addCommand ( QUndoCommand* command );
    virtual void setCursor ( const QCursor& cursor );
    virtual bool snapToGrid() const;
    virtual void gridSize ( qreal* horizontal, qreal* vertical ) const;

    virtual void updateCanvas ( const QRectF& rc );

public Q_SLOTS:
    void update();
    void setDocumentOffset(const QPoint &offset);

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

private:
    class Private;
    QScopedPointer<Private> d;

//     Q_PRIVATE_SLOT(d, void updateSize());
};

#endif // PDFCANVAS_H
