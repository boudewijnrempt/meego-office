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
    virtual void addCommand ( KUndo2Command* command );

    virtual void setCursor ( const QCursor& cursor );
    virtual bool snapToGrid() const;
    virtual void gridSize ( qreal* horizontal, qreal* vertical ) const;

    virtual void updateCanvas ( const QRectF& rc );

    void setSpacing(qreal spacing);

public Q_SLOTS:
    void update();
    void layout();
    void setDocumentOffset(const QPoint &offset);

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

private:
    class Private;
    const QScopedPointer<Private> d;
};

#endif // PDFCANVAS_H
