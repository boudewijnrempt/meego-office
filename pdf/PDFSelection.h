#ifndef PDFSELECTION_H
#define PDFSELECTION_H

#include <QtGui/QGraphicsWidget>

class PDFPage;
class PDFDocument;
class PDFSelection : public QGraphicsWidget
{
Q_OBJECT
public:
    explicit PDFSelection(PDFDocument *document, QGraphicsItem* parentItem = 0, Qt::WindowFlags flags = 0 );
    virtual ~PDFSelection();

    virtual void paint ( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0 );

    void copy();
    void setCurrentPage ( PDFPage *page );
    void setDocumentOffset( QPoint offset );

private:
    class Private;
    const QScopedPointer<Private> d;

    Q_PRIVATE_SLOT(d, void geometryChanged());
    Q_PRIVATE_SLOT(d, void textRequestFinished());
};

#endif // PDFSELECTION_H
