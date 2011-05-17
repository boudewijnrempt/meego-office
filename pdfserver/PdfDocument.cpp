#include "PdfDocument.h"

PdfDocument::PdfDocument(const QString &url)
    : m_url(url)
    , m_pdf(0)
{
    connect(&m_timeout, SIGNAL(timeout()), this, SLOT(timeout()));
    // timeout in half an hour
    m_timeout.singleShot(30 * 1000 * 60, this, SLOT(timeout()));

    m_pdf = Poppler::Document::load(url);

}

PdfDocument::~PdfDocument()
{
    delete m_pdf;
}

bool PdfDocument::isValid()
{
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

int PdfDocument::PageLayout()
{
    if (isValid()) {
        return m_pdf->pageLayout();
    }
    return Poppler::Document::NoLayout;
}

QMap<QString, QString> PdfDocument::infoMap()
{
    QMap<QString, QString> infos;
    if (isValid()) {
        foreach(QString key, m_pdf->infoKeys()) {
            infos.insert(key, m_pdf->info(key));
        }
    }
    return infos;
}
