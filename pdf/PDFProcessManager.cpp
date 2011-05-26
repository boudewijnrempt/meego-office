#include "PDFProcessManager.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QProcess>
#include <QtCore/QDebug>

#include "PDFServerConfig.h"

PDFProcessManager *PDFProcessManager::sm_instance = 0;

class PDFProcessManager::Private
{
public:
    Private() : process(0) { }
    
    void error ( QProcess::ProcessError error );
    
    QProcess *process;
};

PDFProcessManager::PDFProcessManager(QObject *parent)
    : QObject(parent), d(new Private)
{

}

PDFProcessManager::~PDFProcessManager()
{
    if(d->process) {
        d->process->terminate();
        d->process->waitForFinished();
    }
}

void PDFProcessManager::ensureRunning()
{
    if(!d->process) {
        d->process = new QProcess(this);
        connect(d->process, SIGNAL(error(QProcess::ProcessError)), SLOT(error(QProcess::ProcessError)));
        d->process->start(PDF_SERVER_EXECUTABLE, QStringList() << PDF_SERVER_PORT);
        d->process->waitForStarted();
    }
}

PDFProcessManager* PDFProcessManager::instance()
{
    if(!sm_instance) {
        sm_instance = new PDFProcessManager(QCoreApplication::instance());
    }
    return sm_instance;
}

void PDFProcessManager::Private::error ( QProcess::ProcessError error )
{
    qDebug() << "Error in process: Error" << process->errorString();
}

#include "PDFProcessManager.moc"