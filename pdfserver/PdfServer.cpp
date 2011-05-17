#include "PdfServer.h"
#include "PdfServerThread.h"

PdfServer::PdfServer(QObject *parent) :
    QTcpServer(parent)
{
}

void PdfServer::incomingConnection(int socketDescriptor)
{
    PdfServerThread *thread = new PdfServerThread(socketDescriptor, this);
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();
}
