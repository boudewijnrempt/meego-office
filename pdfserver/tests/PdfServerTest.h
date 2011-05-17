#ifndef PDFSERVERTEST
#define PDFSERVERTEST

#include <QObject>

class QNetworkReply;

class PdfServerTest : public QObject {

    Q_OBJECT

private slots:

    void getFinished(QNetworkReply*);
    void output();

    void testInstantiation();
    void testGet();

private:

    bool m_gotReply;
    bool m_replyOk;

};

#endif
