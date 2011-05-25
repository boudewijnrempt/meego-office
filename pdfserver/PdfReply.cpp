#include "PdfReply.h"
#include <QDateTime>
#include <QDebug>

PdfReply::PdfReply()
    : m_status(Undefined_Status),
      m_contentType(Text_ContentType)
{
}

PdfReply::~PdfReply()
{

}

PdfReply::Status PdfReply::status()
{
    return m_status;
}

void PdfReply::setStatus ( PdfReply::Status status )
{
    m_status = status;
}

void PdfReply::setContentType ( PdfReply::ContentType type )
{
    m_contentType = type;
}

void PdfReply::setData ( const QByteArray& data )
{
    m_data = data;
}

void PdfReply::setProperty ( const QString& name, const QString& value )
{
    m_properties.insert(name, value);
}

void PdfReply::setErrorString ( const QString& error )
{
    QString errorString = QString("<h1>%1: %2</h1>").arg(m_status).arg(error);
    m_data = errorString.toUtf8();
    m_contentType = Html_ContentType;
}

qint64 PdfReply::write (QIODevice &out)
{
    QString reply;
    
    switch(m_status) {
        case OK_Status:
            reply.append("HTTP/1.1 200 OK\r\n");
            break;
        case Error_Status:
            reply.append("HTTP/1.1 400 Bad Request\r\n");
            break;
        case NotFound_Status:
            reply.append("HTTP/1.1 404 Not Found\r\n");
            break;
        case InternalError_Status:
            reply.append("HTTP/1.1 500 Internal Server Error\r\n");
            break;
        default:
            qWarning("Invalid reply status, aborting reply.");
            return -1;
    }

    reply.append(QString("Date: %1\r\n").arg(QDateTime::currentDateTime().toString()));
    reply.append("Server: pdfserver 0.1\r\n");
    reply.append("Cache-Control: no-store\r\n");
    reply.append("Connection: close\r\n");

    reply.append(QString("Content-Length: %1\r\n").arg(m_data.length()));

    switch(m_contentType) {
        case Text_ContentType:
            reply.append("Content-Type: text/plain; charset=utf-8\r\n");
            break;
        case Html_ContentType:
            reply.append("Content-Type: text/html; charset=utf-8\r\n");
            break;
        case Image_ContentType:
            reply.append("Content-Type: image/png\r\n");
            break;
        default:
            qWarning("Invalid content type, aborting reply.");
            return -1;
    }

    QHash<QString, QString>::const_iterator itr;
    for(itr = m_properties.begin(); itr != m_properties.end(); ++itr) {
        reply.append(QString("X-PDF-%1: %2\r\n").arg(itr.key(), itr.value()));
    }

    reply.append("\r\n");

    QByteArray replyData = reply.toUtf8();
    replyData.append(m_data);
    return out.write(replyData);
}
