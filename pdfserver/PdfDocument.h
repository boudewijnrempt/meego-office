#ifndef PDFDOCUMENT_H
#define PDFDOCUMENT_H

#include <QTimer>
#include <QMap>

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
    int PageLayout();
    QMap<QString, QString> infoMap();

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
};

#endif // PDFDOCUMENT_H
