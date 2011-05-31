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
#include <QPair>
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

    QCOMPARE(reply->error() , QNetworkReply::UnknownContentError);
}

void PdfServerTest::testOpen()
{
    Q_ASSERT(m_process->state() == QProcess::Running);
    QNetworkAccessManager accessManager;
    
    QNetworkRequest req(QUrl(QString("http://localhost:24098/open?file=%1").arg(PDF_TEST_FILE)));
    QNetworkReply *reply = accessManager.get(req);

    while (!reply->isFinished()) {
        QTest::qWait(10);
    }

    QCOMPARE(reply->error() , QNetworkReply::NoError);

    QCOMPARE(reply->rawHeader("X-PDF-File"), QByteArray(PDF_TEST_FILE));
    QCOMPARE(reply->rawHeader("X-PDF-NumberOfPages"), QByteArray("4"));
    QCOMPARE(reply->rawHeader("X-PDF-PageLayout"), QByteArray("0"));
    QCOMPARE(reply->rawHeader("X-PDF-Title"), QByteArray("Words unsaved document (2011-05-19)"));
}

void PdfServerTest::testGetPage()
{
    QNetworkAccessManager accessManager;
    QNetworkRequest req(QUrl(QString("http://localhost:24098/getpage?file=") + PDF_TEST_FILE + "&page=0&zoom=1.0"));
    QNetworkReply *reply = accessManager.get(req);

    while (!reply->isFinished()) {
        QTest::qWait(10);
    }

    QCOMPARE(reply->error() , QNetworkReply::NoError);

    QCOMPARE(reply->rawHeader("X-PDF-File"), QByteArray(PDF_TEST_FILE));
    QCOMPARE(reply->rawHeader("X-PDF-PageNumber"), QByteArray("0"));
    QCOMPARE(reply->rawHeader("X-PDF-Orientation"), QByteArray("1"));

    QByteArray ba = reply->readAll();
    
    QCOMPARE(ba.size(), reply->header(QNetworkRequest::ContentLengthHeader).toInt());

    QImage page;
    page.loadFromData(ba, "PNG");
    QVERIFY(!page.isNull());
}

void PdfServerTest::testThumbnail()
{
    QNetworkAccessManager accessManager;
    QNetworkRequest req(QUrl(QString("http://localhost:24098/thumbnail?file=") + PDF_TEST_FILE + "&page=1&width=100&height=100" ));
    QNetworkReply *reply = accessManager.get(req);

    while (!reply->isFinished()) {
        QTest::qWait(10);
    }

    QCOMPARE(reply->error() , QNetworkReply::NoError);

    QCOMPARE(reply->rawHeader("X-PDF-File"), QByteArray(PDF_TEST_FILE));
    QCOMPARE(reply->rawHeader("X-PDF-PageNumber"), QByteArray("1"));
    QCOMPARE(reply->rawHeader("X-PDF-Width"), QByteArray("100"));
    QCOMPARE(reply->rawHeader("X-PDF-Height"), QByteArray("100"));

    QByteArray ba = reply->readAll();
    QCOMPARE(ba.size(), reply->header(QNetworkRequest::ContentLengthHeader).toInt());

    QImage page;
    page.loadFromData(ba, "PNG");
    
    QVERIFY(!page.isNull());
    QCOMPARE(page.width(), 100);
    QCOMPARE(page.height(), 100);
}

void PdfServerTest::testSearch()
{
    QNetworkAccessManager accessManager;
    QNetworkRequest req(QUrl(QString("http://localhost:24098/search?file=") + PDF_TEST_FILE + "&page=1&pattern=lorem"));
    QNetworkReply *reply = accessManager.get(req);

    while (!reply->isFinished()) {
        QTest::qWait(10);
    }

    QCOMPARE(reply->error() , QNetworkReply::NoError);

    QCOMPARE(reply->rawHeader("X-PDF-File"), QByteArray(PDF_TEST_FILE));
    QCOMPARE(reply->rawHeader("X-PDF-PageNumber"), QByteArray("1"));
    QCOMPARE(reply->rawHeader("X-PDF-SearchPattern"), QByteArray("lorem"));

    QString results = QString::fromUtf8(reply->readAll());
    QStringList resultList = results.split("\n");
    QList<QRectF> resultRects;
    foreach (QString result, resultList) {
        if (result.contains(",")) {
            QStringList points = result.split(",");
            QRectF rc(QPointF(points.at(0).toFloat(),
                      points.at(1).toFloat()),
                      QPointF(points.at(2).toFloat(),
                      points.at(3).toFloat()));
            resultRects << rc;
        }
    }
    QCOMPARE(resultRects.length(), 5);
}

void PdfServerTest::testText()
{
    QNetworkAccessManager accessManager;
    QNetworkRequest req(QUrl(QString("http://localhost:24098/text?file=") + PDF_TEST_FILE + "&page=1&left=143&top=196.631&right=181.167&bottom=209.203"));
    QNetworkReply *reply = accessManager.get(req);

    while (!reply->isFinished()) {
        QTest::qWait(10);
    }

    QCOMPARE(reply->error() , QNetworkReply::NoError);

    QCOMPARE(reply->rawHeader("X-PDF-File"), QByteArray(PDF_TEST_FILE));
    QCOMPARE(reply->rawHeader("X-PDF-PageNumber"), QByteArray("1"));
    QCOMPARE(reply->rawHeader("X-PDF-LeftTop"), QByteArray("143,196.631"));
    QCOMPARE(reply->rawHeader("X-PDF-RightBottom"), QByteArray("181.167,209.203"));

    QByteArray data = reply->readAll();
    QCOMPARE(data, QByteArray("Lorem"));
}

void PdfServerTest::testNoLinks()
{
    QNetworkAccessManager accessManager;
    QNetworkRequest req(QUrl(QString("http://localhost:24098/links?file=") + PDF_TEST_FILE + "&page=1"));
    QNetworkReply *reply = accessManager.get(req);

    while (!reply->isFinished()) {
        QTest::qWait(10);
    }

    QCOMPARE(reply->error() , QNetworkReply::NoError);

    QCOMPARE(reply->rawHeader("X-PDF-File"), QByteArray(PDF_TEST_FILE));
    QCOMPARE(reply->rawHeader("X-PDF-PageNumber"), QByteArray("1"));

    QString s = QString::fromUtf8(reply->readAll());
    QVERIFY(s.isEmpty());
}

void PdfServerTest::testLink()
{
    QNetworkAccessManager accessManager;
    QNetworkRequest req(QUrl(QString("http://localhost:24098/links?file=") + PDF_TEST_FILE_LINK + "&page=0"));
    QNetworkReply *reply = accessManager.get(req);

    while (!reply->isFinished()) {
        QTest::qWait(10);
    }

    QCOMPARE(reply->error() , QNetworkReply::NoError);

    QCOMPARE(reply->rawHeader("X-PDF-File"), QByteArray(PDF_TEST_FILE_LINK));
    QCOMPARE(reply->rawHeader("X-PDF-PageNumber"), QByteArray("0"));
    QCOMPARE(reply->rawHeader("X-PDF-NumberOfLinks"), QByteArray("1"));

    QString results = QString::fromUtf8(reply->readAll());
    QStringList resultList = results.split("\n");

    QVector<QRectF> linkrects;
    QVector<QPair<QString,QString> > links;

    foreach (QString result, resultList) {
        if (result.contains(",")) {
            QStringList points = result.split(",");
            QRectF rc(QPointF(points.at(0).toFloat(),
                      points.at(1).toFloat()),
                      QPointF(points.at(2).toFloat(),
                      points.at(3).toFloat()));
            linkrects << rc;
            links << QPair<QString,QString>(points.at(4), points.at(5));
        }
    }
    QCOMPARE(linkrects.size(), 1);
    QCOMPARE(links.size(), 1);
    QCOMPARE(links.first().first, QString("url"));
    QCOMPARE(links.first().second, QString("http://www.valdyas.org/"));
}


QTEST_MAIN(PdfServerTest)

#include "PdfServerTest.moc"