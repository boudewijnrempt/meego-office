#ifndef PDFSERVERTEST
#define PDFSERVERTEST

#include <QObject>

class QNetworkReply;

class PdfServerTest : public QObject {

    Q_OBJECT

private slots:


    void testInstantiation();

    void getFinished(QNetworkReply*);
    void testGet();
    void init();

private:

    bool m_gotReply;
    bool m_replyOk;

};

#endif
