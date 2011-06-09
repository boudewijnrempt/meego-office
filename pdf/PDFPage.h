#ifndef PDFPAGE_H
#define PDFPAGE_H

#include <QObject>

class QPainter;
class QRectF;
class QImage;
class QTime;
class PDFDocument;
class PDFPage : public QObject
{
Q_OBJECT
public:
    explicit PDFPage (PDFDocument *document, int pageNumber);
    virtual ~PDFPage();

    int pageNumber();

    qreal positionInDocument();
    qreal width();
    qreal height();
    QRectF boundingRect();

    int byteSize();

    QImage image( int width, int height);

    QTime lastVisibleTime();

public Q_SLOTS:
    void load();
    void unload();
    void setPositionInDocument(qreal position);

    void paint(QPainter *painter, const QRectF &target);

private:
    class Private;
    const QScopedPointer<Private> d;

    Q_PRIVATE_SLOT(d, void requestFinished(QNetworkReply *reply));
};

#endif // PDFPAGE_H
