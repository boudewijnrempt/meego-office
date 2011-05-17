#include "PdfServerThread.h"
#include "PdfDocumentCache.h"

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
        emit error(socket.error());
    }
}
