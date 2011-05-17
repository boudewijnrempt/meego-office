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
    m_gotReply = true;
    m_replyOk = (reply->error() == QNetworkReply::NoError);
}

void PdfServerTest::testGet()
{
    // Start the server process
    QProcess process;
    QStringList arguments;
    arguments << "24098";
    process.start(PDF_SERVER_EXECUTABLE, arguments);
    process.waitForStarted();

    // Get a 200 OK back
    QNetworkAccessManager accessManager;
    connect(&accessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(getFinished(QNetworkReply*)));

    QNetworkRequest req(QUrl("http://localhost:24098/bla"));
    accessManager.get(req);

    while (!m_gotReply) {
        QTest::qWait(10);
    }

    process.close();
    Q_ASSERT(process.exitCode() == 0);

    Q_ASSERT(m_replyOk);

}

void PdfServerTest::init()
{
    QProcess process;
    process.execute("killall -9 pdfserver");
}



QTEST_MAIN(PdfServerTest)
