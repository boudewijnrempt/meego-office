#ifndef PDFDOCUMENTCACHE_H
#define PDFDOCUMENTCACHE_H

#include <QObject>
#include <QMutex>
#include <QMap>

class PdfDocument;

/**
 * Keeps the cache of open documents
 */
class PdfDocumentCache : public QObject
{
    Q_OBJECT
public:
    PdfDocumentCache();
    virtual ~PdfDocumentCache();

    // creates or returns the requested document
    PdfDocument *document(const QString &url);

public slots:

    void deleteDocument(const QString &url);

private:

    QMutex m_documentMutex;
    QMap<QString, PdfDocument*> m_documents;
};

#endif // PDFDOCUMENTCACHE_H
