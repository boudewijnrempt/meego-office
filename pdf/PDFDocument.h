#ifndef PDFDOCUMENT_H
#define PDFDOCUMENT_H

#include <QtCore/QObject>
#include <QtCore/QUrl>
#include <QtGui/QImage>

class PDFDocument : public QObject
{
Q_OBJECT
public:
    struct PDFPage {
        int pageNumber;
        QImage image;
        int orientation;
        qreal width;
        qreal height;
    };
    
    PDFDocument(QObject *parent, const QUrl &path = QUrl());
    virtual ~PDFDocument();

    QUrl path();

    int pageCount();
    int pageLayout();
    qreal width();
    qreal height();

    QSizeF documentSize();

    PDFPage *page(int pageNumber, bool requestIfNotAvailable = false);

public Q_SLOTS:
    void open();
    void setPath(const QUrl &path);

    void requestPage(int page, qreal zoom = 1.0);

Q_SIGNALS:
    void opened();
    void newPage(int page);
    void documentSizeChanged(const QSizeF &size);
    
private:
    class Private;
    QScopedPointer<Private> d;

    Q_PRIVATE_SLOT(d, void requestFinished(QNetworkReply *reply));
};

#endif // PDFDOCUMENT_H
