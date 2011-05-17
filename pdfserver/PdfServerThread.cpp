#include "PdfServerThread.h"

PdfServerThread::PdfServerThread(int socketDescriptor, QObject *parent) :
    QThread(parent)
{
}
