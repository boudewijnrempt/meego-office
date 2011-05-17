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
