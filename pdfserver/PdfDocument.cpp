#include "PdfDocument.h"

PdfDocument::PdfDocument(const QString &url)
    : m_url(url)
{
    connect(&m_timeout, SIGNAL(timeout()), this, SLOT(timeout()));
    // timeout in half an hour
    m_timeout.singleShot(30 * 1000 * 60, this, SLOT(timeout()));

}

void PdfDocument::timeout()
{
    emit timedout(m_url);
}
