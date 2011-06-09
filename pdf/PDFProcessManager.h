#ifndef PDFPROCESSMANAGER_H
#define PDFPROCESSMANAGER_H

#include <QObject>

class PDFProcessManager : public QObject
{
Q_OBJECT
public:
    explicit PDFProcessManager ( QObject* parent = 0 );
    virtual ~PDFProcessManager();

    void ensureRunning();

    static PDFProcessManager * instance();

private:
    class Private;
    const QScopedPointer<Private> d;

    Q_PRIVATE_SLOT(d, void error(QProcess::ProcessError error));

    static PDFProcessManager *sm_instance;
};

#endif // PDFPROCESSMANAGER_H
