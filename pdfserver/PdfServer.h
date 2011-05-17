#ifndef PDFSERVER_H
#define PDFSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include "PdfDocumentCache.h"

class PdfServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit PdfServer(QObject *parent = 0);
    void incomingConnection(int socketDescriptor);
private slots:
    void serverThreadError(QTcpSocket::SocketError error);
private:
    PdfDocumentCache m_documentCache;

};

#endif // PDFSERVER_H
