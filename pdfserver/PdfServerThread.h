#ifndef PDFSERVERTHREAD_H
#define PDFSERVERTHREAD_H

#include <QThread>
#include <QTcpSocket>

class PdfDocumentCache;

class PdfServerThread : public QThread
{
    Q_OBJECT

public:

    explicit PdfServerThread(PdfDocumentCache *documentCache, int socketDescriptor, QObject *parent = 0);
    virtual void run();

signals:

    void error(QTcpSocket::SocketError error);

private:
    int m_socketDescriptor;
    PdfDocumentCache *m_documentCache;
};

#endif // PDFSERVERTHREAD_H
