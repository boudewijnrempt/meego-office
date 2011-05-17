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

    QString url() { return m_url; }

signals:

    void sigError(QTcpSocket::SocketError error);

private:

    QByteArray open(const QStringList &uri);
    QByteArray getpage(const QStringList &uri);
    QByteArray thumbnail(const QStringList &uri);
    QByteArray search(const QStringList &uri);
    QByteArray text(const QStringList &uri);
    QByteArray links(const QStringList &uri);

    int m_socketDescriptor;
    PdfDocumentCache *m_documentCache;
    QString m_url;
};

#endif // PDFSERVERTHREAD_H
