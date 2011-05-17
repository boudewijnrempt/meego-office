#ifndef PDFSERVERTHREAD_H
#define PDFSERVERTHREAD_H

#include <QThread>

class PdfServerThread : public QThread
{
    Q_OBJECT
public:
    explicit PdfServerThread(int socketDescriptor, QObject *parent = 0);

signals:

public slots:

};

#endif // PDFSERVERTHREAD_H
