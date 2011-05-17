#include "PdfServerThread.h"
#include "PdfDocumentCache.h"
#include <QDebug>
#include <QRegExp>
#include <QStringList>
#include <QTextStream>
#include <QByteArray>

PdfServerThread::PdfServerThread(PdfDocumentCache *documentCache, int socketDescriptor, QObject *parent)
   : QThread(parent)
   , m_socketDescriptor(socketDescriptor)
   , m_documentCache(documentCache)
{
}

void PdfServerThread::run()
{
    QTcpSocket socket;
    if (!socket.setSocketDescriptor(m_socketDescriptor)) {
        qDebug() << "Got an error setting socketdescriptor" << socket.error();
        emit sigError(socket.error());
    }

    // Make sure the headers are all here
    while(!socket.waitForReadyRead(-1)) {
        sleep(10);
    }
    // first line is GET
    if (socket.canReadLine()) {
        QString line = socket.readLine();
        QStringList tokens = line.split(QRegExp("[ \r\n][ \r\n]*"));
        qDebug() << tokens;
        if (tokens.length() < 2) {
            qDebug() << "Not a GET request";
            return;
        }
    }

    QString reply =  "HTTP/1.0 200 Ok\r\n"
            "Content-Type: text/html; charset=\"ascii\"\r\n"
            "\r\n"
            "<h1>Nothing to see here</h1>\n";

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << reply.toAscii();

    socket.write(block);
    socket.disconnectFromHost();
    socket.waitForDisconnected();
}
