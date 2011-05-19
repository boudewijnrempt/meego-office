#ifndef PDFSERVERTEST
#define PDFSERVERTEST

#include <QObject>
class QProcess;

class QNetworkReply;

class PdfServerTest : public QObject {

    Q_OBJECT

private slots:

    // to kill -9 any and all running pdfservers and then start one
    void initTestCase();
    // close the process
    void cleanupTestCase();

    void testGet();

    void testOpen();

    void testGetPage();

    void testThumbnail();

    void testSearch();

    void testText();

    void testLinks();
private:

    QProcess *m_process;
};

#endif
