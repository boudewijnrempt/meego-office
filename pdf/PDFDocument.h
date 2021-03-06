#ifndef PDFDOCUMENT_H
#define PDFDOCUMENT_H

#include <QtCore/QObject>
#include <QtCore/QUrl>
#include <QtGui/QImage>

#define PDF_SCALING_FACTOR 1.31f

class QNetworkRequest;
class QNetworkAccessManager;
class PDFPage;
class PDFDocument : public QObject
{
Q_OBJECT
public:
    PDFDocument(QObject *parent, const QUrl &path = QUrl());
    virtual ~PDFDocument();

    QUrl path();
    QNetworkAccessManager *networkManager();

    int pageCount();
    int pageLayout();

    QSizeF documentSize();

    PDFPage *page(int pageNumber);
    PDFPage *pageAt(const QPointF &location, const QMatrix &scaling);
    QList<PDFPage*> allPages();

    QList<PDFPage*> visiblePages( const QRectF& viewRect, const QMatrix& scaling);

    QNetworkRequest buildRequest(const QString &command, const QString &arguments = QString());

public Q_SLOTS:
    void open();
    void setPath(const QUrl &path);
    
    void setMemoryLimit(int bytes);
    void setMinimumCachedPages(int pages);

    void updateCache();

Q_SIGNALS:
    void opened();
    void documentSizeChanged(const QSizeF &size);
    
private:
    class Private;
    const QScopedPointer<Private> d;

    Q_PRIVATE_SLOT(d, void openRequestFinished());
};

#endif // PDFDOCUMENT_H
