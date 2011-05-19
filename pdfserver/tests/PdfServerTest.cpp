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
#include <QBuffer>
#include <QByteArray>
#include <QImage>
#include <QMap>

void PdfServerTest::initTestCase()
{
    QProcess p;
    p.execute("killall -9 pdfserver");

    // Start the server process
    m_process = new QProcess();
    QStringList arguments;
    arguments << "24098";
    m_process->start(PDF_SERVER_EXECUTABLE, arguments);
    m_process->waitForStarted();

}

void PdfServerTest::cleanupTestCase()
{
    m_process->close();
    m_process->waitForFinished();
    QCOMPARE(m_process->exitCode(), 0);
    delete m_process;
}

void PdfServerTest::testGet()
{
    // Get a 400 BAD REQUEST back -- it's 299 in QNetWorkReply
    QNetworkAccessManager accessManager;
    QNetworkRequest req(QUrl("http://localhost:24098/bla"));
    QNetworkReply *reply = accessManager.get(req);

    while (!reply->isFinished()) {
        QTest::qWait(10);
    }

//    qDebug() << reply->rawHeaderPairs();
//    qDebug() << reply->readAll();
//    qDebug() << reply->error() << reply->errorString();
    QCOMPARE(reply->error() , QNetworkReply::UnknownContentError);

}

void PdfServerTest::testOpen()
{
    Q_ASSERT(m_process->state() == QProcess::Running);
    QNetworkAccessManager accessManager;
    QNetworkRequest req(QUrl(QString("http://localhost:24098/open?") + PDF_TEST_FILE));
    QNetworkReply *reply = accessManager.get(req);

    while (!reply->isFinished()) {
        QTest::qWait(10);
    }
    QString s = QString::fromUtf8(reply->readAll());
    QStringList lines = s.split("\n");
    QMap<QString,QString> infos;
    foreach(QString line, lines) {
        if (line.contains("=")) {
            QStringList tokens = line.split("=");
            infos.insert(tokens[0], tokens[1]);
        }
    }
    QCOMPARE(infos["url"], QString(PDF_TEST_FILE));
    QCOMPARE(infos["numberofpages"], QString("4"));
    QCOMPARE(infos["pagelayout"], QString("0"));
    QCOMPARE(infos["Title"], QString("Words unsaved document (2011-05-19)"));

    QCOMPARE(reply->error() , QNetworkReply::NoError);

}

void PdfServerTest::testGetPage()
{
    QNetworkAccessManager accessManager;
    QNetworkRequest req(QUrl(QString("http://localhost:24098/getpage?") + PDF_TEST_FILE + "?0?1.0"));
    QNetworkReply *reply = accessManager.get(req);

    while (!reply->isFinished()) {
        QTest::qWait(10);
    }

    QByteArray ba = reply->readAll();

    QVERIFY(ba.contains("-----------"));
    int sepPos = ba.indexOf("-----------");
    QString s = QString::fromUtf8(ba.left(sepPos));
    QStringList lines = s.split("\n");
    QMap<QString,QString> infos;
    foreach(QString line, lines) {
        if (line.contains("=")) {
            QStringList tokens = line.split("=");
            infos.insert(tokens[0], tokens[1]);
        }
    }

    QCOMPARE(infos["url"], QString(PDF_TEST_FILE));
    QCOMPARE(infos["pagenumber"], QString("0"));
    QCOMPARE(infos["orientation"], QString("1"));

    int imagesize = infos["imagesize"].toInt();
    QByteArray png(ba.right(imagesize));
    QCOMPARE(png.size(), imagesize);

    QBuffer buf(&png);
    buf.open(QIODevice::ReadOnly);
    QImage page;
    page.load(&buf, "PNG");
    QVERIFY(!page.isNull());

    QCOMPARE(reply->error() , QNetworkReply::NoError);

}

void PdfServerTest::testThumbnail()
{
    QNetworkAccessManager accessManager;
    QNetworkRequest req(QUrl("http://localhost:24098/bla"));
    QNetworkReply *reply = accessManager.get(req);

    while (!reply->isFinished()) {
        QTest::qWait(10);
    }

//    qDebug() << reply->rawHeaderPairs();
//    qDebug() << reply->readAll();
//    qDebug() << reply->error() << reply->errorString();
    QCOMPARE(reply->error() , QNetworkReply::NoError);

}

void PdfServerTest::testSearch()
{
    QNetworkAccessManager accessManager;
    QNetworkRequest req(QUrl("http://localhost:24098/bla"));
    QNetworkReply *reply = accessManager.get(req);

    while (!reply->isFinished()) {
        QTest::qWait(10);
    }

//    qDebug() << reply->rawHeaderPairs();
//    qDebug() << reply->readAll();
//    qDebug() << reply->error() << reply->errorString();
    QCOMPARE(reply->error() , QNetworkReply::NoError);

}

void PdfServerTest::testText()
{
    QNetworkAccessManager accessManager;
    QNetworkRequest req(QUrl("http://localhost:24098/bla"));
    QNetworkReply *reply = accessManager.get(req);

    while (!reply->isFinished()) {
        QTest::qWait(10);
    }

//    qDebug() << reply->rawHeaderPairs();
//    qDebug() << reply->readAll();
//    qDebug() << reply->error() << reply->errorString();
    QCOMPARE(reply->error() , QNetworkReply::NoError);

}

void PdfServerTest::testLinks()
{
    QNetworkAccessManager accessManager;
    QNetworkRequest req(QUrl("http://localhost:24098/bla"));
    QNetworkReply *reply = accessManager.get(req);

    while (!reply->isFinished()) {
        QTest::qWait(10);
    }

//    qDebug() << reply->rawHeaderPairs();
//    qDebug() << reply->readAll();
//    qDebug() << reply->error() << reply->errorString();
    QCOMPARE(reply->error() , QNetworkReply::NoError);

}


QTEST_MAIN(PdfServerTest)
