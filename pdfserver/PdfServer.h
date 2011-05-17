#ifndef PDFSERVER_H
#define PDFSERVER_H

#include <QTcpServer>

class PdfServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit PdfServer(QObject *parent = 0);
    void incomingConnection(int socketDescriptor);

};

#endif // PDFSERVER_H
