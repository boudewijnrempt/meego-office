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
}

PdfDocument *PdfDocumentCache::document(const QString &url)
{
    QMutexLocker locker(&m_documentMutex);
    PdfDocument *document;
    if (m_documents.contains(url)) {
        document = m_documents[url];
    }
    else {
        document = new PdfDocument(url);
        m_documents.insert(url, document);
    }
    return document;
}

void PdfDocumentCache::deleteDocument(const QString &url)
{
    QMutexLocker locker(&m_documentMutex);
    if (m_documents.contains(url)) {
        delete m_documents.take(url);
    }
}
