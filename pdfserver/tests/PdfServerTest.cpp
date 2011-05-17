#include "PdfServerTest.h"

#include <QDebug>
#include <QProcess>
#include <QTest>
#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QTest>

void PdfServerTest::testInstantiation()
{
    // Start the server process
    QProcess process;
    QStringList arguments;
    arguments << "24098";
    process.start(PDF_SERVER_EXECUTABLE, arguments);
    process.waitForStarted();
    process.close();
    Q_ASSERT(process.exitCode() == 0);

}

void PdfServerTest::getFinished(QNetworkReply *reply)
{
    qDebug() << "getFinished";
    m_gotReply = true;
    m_replyOk = (reply->error() == QNetworkReply::NoError);
}

void PdfServerTest::output()
{
    QProcess *process = qobject_cast<QProcess*>(sender());
    if (process) {
        QString s = process->readAllStandardError();
        if (!s.isEmpty()) qDebug() << "err" << s;
        s = process->readAllStandardOutput();
        if (!s.isEmpty()) qDebug() << "out" << s;
    }
}

void PdfServerTest::testGet()
{
    // Start the server process
    QProcess process;
    connect(&process, SIGNAL(readyReadStandardError()), this, SLOT(output()));
    connect(&process, SIGNAL(readyReadStandardOutput()), this, SLOT(output()));
    QStringList arguments;
    arguments << "24098";
    process.start(PDF_SERVER_EXECUTABLE, arguments);
    process.waitForStarted();

    // Get a 200 OK back
    QNetworkAccessManager accessManager;

    QNetworkRequest req(QUrl("http://localhost:4242/bla"));
    accessManager.get(req);

    while (!m_gotReply) {
        qDebug() << ".";
        QTest::qWait(10);
    }

    Q_ASSERT(m_replyOk);

    process.close();
    Q_ASSERT(process.exitCode() == 0);

}

QTEST_MAIN(PdfServerTest)
