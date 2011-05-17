#ifndef PDFDOCUMENT_H
#define PDFDOCUMENT_H

#include <QTimer>

/**
 * Wrapper around a pdf document. Removes itself after a certain period of time
 */
class PdfDocument : public QObject
{
    Q_OBJECT
public:
    PdfDocument(const QString &url);

private slots:

    // not accesses for a certain period, let's ask the cache to delete us
    void timeout();

signals:

    // emitted when this document can be removed from the cache
    void timedout(const QString);

private:
    QString m_url;
    QTimer m_timeout;
};

#endif // PDFDOCUMENT_H
