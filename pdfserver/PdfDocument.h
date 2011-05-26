#ifndef PDFDOCUMENT_H
#define PDFDOCUMENT_H

#include <QTimer>
#include <QMap>
#include <QMutex>

#include <poppler/qt4/poppler-qt4.h>

/**
 * Wrapper around a pdf document. Removes itself after a certain period of time
 */
class PdfDocument : public QObject
{
    Q_OBJECT
public:
    PdfDocument(const QString &url);
    virtual ~PdfDocument();

    bool isValid();

    int numberOfPages();
    int pageLayout();
    QMap<QString, QString> infoMap();

    Poppler::Page *page(int pageNumber);

private slots:

    // not accesses for a certain period, let's ask the cache to delete us
    void timeout();

signals:

    // emitted when this document can be removed from the cache
    void timedout(const QString);

private:
    QString m_url;
    QTimer m_timeout;
    Poppler::Document *m_pdf;
    QMutex m_mutex;
    QMap<int, Poppler::Page*> m_pageCache;
};

#endif // PDFDOCUMENT_H
