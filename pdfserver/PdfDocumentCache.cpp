#include "PdfDocumentCache.h"
#include "PdfDocument.h"

#include <QMutexLocker>

PdfDocumentCache::PdfDocumentCache()
{
}

PdfDocumentCache::~PdfDocumentCache()
{
    QMutexLocker locker(&m_documentMutex);
    qDeleteAll(m_documents);
    locker.unlock();
}

PdfDocument *PdfDocumentCache::document(const QString &url)
{
    QMutexLocker locker(&m_documentMutex);
    PdfDocument *document = 0;
    if (m_documents.contains(url)) {
        document = m_documents[url];
    }
    else {
        document = new PdfDocument(url);
        if (document) {
            m_documents.insert(url, document);
        }
    }
    locker.unlock();
    return document;
}

void PdfDocumentCache::deleteDocument(const QString &url)
{
    QMutexLocker locker(&m_documentMutex);
    if (m_documents.contains(url)) {
        delete m_documents.take(url);
    }
    locker.unlock();
}

#include "PdfDocumentCache.moc"
