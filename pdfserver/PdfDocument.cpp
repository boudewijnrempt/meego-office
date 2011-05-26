#include "PdfDocument.h"
#include <QDebug>
#include <QMutexLocker>

PdfDocument::PdfDocument(const QString &url)
    : m_url(url)
    , m_pdf(0)
{
    connect(&m_timeout, SIGNAL(timeout()), this, SLOT(timeout()));
    // timeout in half an hour
    m_timeout.singleShot(30 * 1000 * 60, this, SLOT(timeout()));

    m_pdf = Poppler::Document::load(url);
    m_pdf->setRenderHint(Poppler::Document::Antialiasing, true);
    m_pdf->setRenderHint(Poppler::Document::TextAntialiasing, true);

}

PdfDocument::~PdfDocument()
{
    qDeleteAll(m_pageCache);
    delete m_pdf;
}

bool PdfDocument::isValid()
{
    QMutexLocker lock(&m_mutex);
    return m_pdf && !m_pdf->isLocked();
}

void PdfDocument::timeout()
{
    emit timedout(m_url);
}

int PdfDocument::numberOfPages()
{
    if (isValid()) {
        return m_pdf->numPages();
    }
    return 0;
}

int PdfDocument::pageLayout()
{
    if (isValid()) {
        return m_pdf->pageLayout();
    }
    return Poppler::Document::NoLayout;
}

QMap<QString, QString> PdfDocument::infoMap()
{
    QMutexLocker lock(&m_mutex);
    QMap<QString, QString> infos;
    if (m_pdf && !m_pdf->isLocked()) {
        QStringList keys = m_pdf->infoKeys();
        foreach(const QString &key, keys) {
            infos.insert(key, m_pdf->info(key));
        }
    }
    return infos;
}

Poppler::Page *PdfDocument::page(int pageNumber)
{
    QMutexLocker lock(&m_mutex);
    Poppler::Page *page = 0;
    if (m_pdf && !m_pdf->isLocked()) {
        if (m_pageCache.contains(pageNumber)) {
            page = m_pageCache[pageNumber];
        }
        else {
            page = m_pdf->page(pageNumber);
            if (page) {
                m_pageCache.insert(pageNumber, page);
            }
        }
    }
    return page;
}
