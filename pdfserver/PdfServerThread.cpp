#include "PdfServerThread.h"
#include "PdfDocument.h"
#include "PdfDocumentCache.h"
#include <QDebug>
#include <QRegExp>
#include <QStringList>
#include <QTextStream>
#include <QByteArray>
#include <QImage>
#include <QBuffer>

#ifdef Q_WS_X11
#include <QtGui/QX11Info>
#else
#include <QDesktopWidget>
#endif



PdfServerThread::PdfServerThread(PdfDocumentCache *documentCache, int socketDescriptor, QObject *parent)
    : QThread(parent)
    , m_socketDescriptor(socketDescriptor)
    , m_documentCache(documentCache)
{
}

void PdfServerThread::run()
{
    QTcpSocket socket;
    if (!socket.setSocketDescriptor(m_socketDescriptor)) {
        qDebug() << "Got an error setting socketdescriptor" << socket.error();
        emit sigError(socket.error());
    }

    // Make sure the headers are all here
    while(!socket.waitForReadyRead(-1)) {
        sleep(10);
    }

    QByteArray answer;
    QString socketError;

    // first line is GET
    if (socket.canReadLine()) {
        QString line = socket.readLine();
        QStringList tokens = line.split(QRegExp("[ \r\n][ \r\n]*"));
        if (tokens.length() < 2 || tokens[0] != "GET") {
            socketError = "Not a GET request" + line;
        }
        QStringList uri = tokens[1].split("?");
        if (tokens.length() < 2) {
            socketError = "No Command" + line;
        }

        if (socketError.isEmpty()) {

            QString command = uri[0];
            if (command == "/open") {
                answer = open(uri);
                if (answer.length() == 0) {
                    socketError = "Could not open document: " + uri.join("?");
                }
            }
            else if (command == "/getpage") {
                answer = getpage(uri);
                if (answer.length() == 0) {
                    socketError = "Could not get page: " + uri.join("?");
                }
            }
            else if (command == "/thumbnail") {
                answer = thumbnail(uri);
                if (answer.length() == 0) {
                    socketError = "Could not get thumbnail: " + uri.join("?");
                }
            }
            else if (command == "/search") {
                answer = search(uri);
                if (answer.length() == 0) {
                    socketError = "Could not execute search: " + uri.join("?");
                }
            }
            else if (command == "/text") {
                answer = text(uri);
                if (answer.length() == 0) {
                    socketError = "Could not get text: " + uri.join("?");
                }
            }
            else if (command == "/links") {
                answer = links(uri);
                if (answer.length() == 0) {
                    socketError = "Could not get links: " + uri.join("?");
                }
            }
            else {
                socketError = "Illegal command " + uri.join("?");
            }
        }
    }
    else {
        socketError = "Could not read line";
    }
    if (socketError.isEmpty()) {
        QString reply("HTTP/1.1 200 Ok\r\n"
                      "Content-Type: text/html; charset=\"utf-8\"\r\n"
                      "\r\n");
        qDebug() << reply;
        socket.write(reply.toUtf8());
        qDebug() << answer;
        socket.write(answer);
    }
    else {
        qDebug() << socketError;
        QString reply("HTTP/1.1 400 Bad Request\r\n"
                      "Content-Type: text/html; charset=\"utf-8\"\r\n"
                      "\r\n");
        socket.write(reply.toUtf8());
        socket.write(QString("<h1>").toUtf8());
        socket.write(socketError.toUtf8());
        socket.write(QString("</h1>").toUtf8());
    }
    socket.flush();
    socket.close();
}


QByteArray PdfServerThread::open(const QStringList &uri)
{
    QByteArray answer;

    if (uri.length() != 2) return answer;

    PdfDocument *doc = m_documentCache->document(uri[1]);
    if (!doc || !doc->isValid()) {
        return answer;
    }

    QString s("url=%1\n"
              "numberofpages=%2\n"
              "pagelayout=%3\n");

    s = s.arg(uri[1]).arg(doc->numberOfPages()).arg(doc->pageLayout());
    QMap<QString, QString> infomap = doc->infoMap();
    foreach(const QString key, infomap.keys()) {
        s.append(QString("%1=%2\n").arg(key).arg(infomap[key]));
    }

    answer = s.toUtf8();
    return answer;
}

QByteArray PdfServerThread::getpage(const QStringList &uri)
{
    QByteArray answer;

    if (uri.length() != 4) return answer;

    qDebug() << 1;

    PdfDocument *doc = m_documentCache->document(uri[1]);
    if (!doc || !doc->isValid()) {
        return answer;
    }

    int pageNumber = uri[2].toInt();

    Poppler::Page *page = doc->page(pageNumber);
    if (!page) {
        return answer;
    }

    qreal zoomlevel = uri[3].toFloat();

    qreal dpiX, dpiY;
    dpi(dpiX, dpiY, zoomlevel);

    // XXX: rendering quality isn't what it should be. Probably some error here
    QImage img = page->renderToImage(dpiX, dpiY);
    QByteArray imageBytes;
    QBuffer buf(&imageBytes);
    buf.open(QIODevice::WriteOnly | QIODevice::Append);
    img.save(&buf, "PNG");

    QString s("url=%1\n"
              "pagenumber=%2\n"
              "zoomlevel=%3\n"
              "orientation=%4\n"
              "imagesize=%5"
              "-----------\n");
    s = s.arg(uri[1]).arg(pageNumber).arg(zoomlevel).arg(page->orientation()).arg(imageBytes.size());

    answer = s.toUtf8();
    answer += imageBytes;

    return answer;
}

QByteArray PdfServerThread::thumbnail(const QStringList &uri)
{
    QByteArray answer;

    if (uri.length() != 5) return answer;

    PdfDocument *doc = m_documentCache->document(uri[1]);
    if (!doc || !doc->isValid()) {
        delete doc;
        return answer;
    }

    int pageNumber = uri[2].toInt();
    Poppler::Page *page = doc->page(pageNumber);
    if (!page) {
        return answer;
    }

    QSize thumbsize(uri[3].toInt(), uri[4].toInt());

    QImage thumb = page->thumbnail();
    if (thumb.isNull()) {

        qreal dpiX, dpiY;
        dpi(dpiX, dpiY, 1.0);
        QSizeF pageSize = page->pageSizeF();
        qreal fakeDpiX = thumbsize.width() * dpiX / pageSize.width();
        qreal fakeDpiY = thumbsize.height() * dpiY / pageSize.height();

        thumb = page->renderToImage(fakeDpiX, fakeDpiY);
    }

    if (thumb.size() != thumbsize) {
        thumb = thumb.scaled(thumbsize);
    }
    QByteArray imageBytes;
    QBuffer buf(&imageBytes);
    buf.open(QIODevice::WriteOnly | QIODevice::Append);
    thumb.save(&buf, "PNG");


    QString s("url=%1\n"
              "pagenumber=%2\n"
              "width=%3\n"
              "height=%4\n"
              "imagesize=%5"
              "-----------\n"
              );
    s = s.arg(uri[1]).arg(pageNumber).arg(thumbsize.width()).arg(thumbsize.height()).arg(imageBytes.size());

    answer = s.toUtf8();
    answer += imageBytes;

    return answer;
}

QByteArray PdfServerThread::search(const QStringList &uri)
{
    QByteArray answer;

    if (uri.length() != 4) return answer;

    PdfDocument *doc = m_documentCache->document(uri[1]);
    if (!doc || !doc->isValid()) {
        return answer;
    }

    int pageNumber = uri[2].toInt();
    Poppler::Page *page = doc->page(pageNumber);
    if (!page) {
        return answer;
    }

    QString searchString = uri[3];
    if (searchString.isEmpty()) {
        return answer;
    }


    QString s("url=%1\n"
              "pagenumber=%1\n"
              "searchstring=%2\n"
              "-----------\n");

    s = s.arg(uri[1]).arg(pageNumber).arg(searchString);

    bool found = true;
    double top, bottom, left, right = 0.0;

    while(found) {
        found = page->search(searchString, left, top, right, bottom,
                             Poppler::Page::NextResult, Poppler::Page::CaseInsensitive);
        if (found) {
            s.append("%1,%2,%3,%4\n");
            s.arg(left).arg(top).arg(right).arg(bottom);
        }
    }

    answer = s.toUtf8();
    return answer;
}

QByteArray PdfServerThread::text(const QStringList &uri)
{
    QByteArray answer;

    if (uri.length() != 7) return answer;

    PdfDocument *doc = m_documentCache->document(uri[1]);
    if (!doc || !doc->isValid()) {
        return answer;
    }

    int pageNumber = uri[2].toInt();
    Poppler::Page *page = doc->page(pageNumber);
    if (!page) {
        return answer;
    }

    int left = uri[3].toInt();
    int top = uri[4].toInt();
    int right = uri[5].toInt();
    int bottom = uri[6].toInt();

    QString text = page->text(QRectF(QPointF(left, top), QPointF(right, bottom)));

    QString s("url=%1\n"
              "pagenumber=%2\n"
              "left=%3\n"
              "top=%4\n"
              "right=%5\n"
              "bottom=%6\n"
              "text=%7");
    s = s.arg(uri[1])
            .arg(pageNumber)
            .arg(left)
            .arg(top)
            .arg(right)
            .arg(bottom)
            .arg(text);

    answer = s.toUtf8();
    return answer;
}

QByteArray PdfServerThread::links(const QStringList &uri)
{
    QByteArray answer;

    if (uri.length() != 3) return answer;

    PdfDocument *doc = m_documentCache->document(uri[1]);
    if (!doc || !doc->isValid()) {
        return answer;
    }

    int pageNumber = uri[2].toInt();
    Poppler::Page *page = doc->page(pageNumber);
    if (!page) {
        return answer;
    }

    QString s("url=%1\n"
              "pagenumber=%2\n"
              "-----------\n");
    s = s.arg(uri[1])
            .arg(pageNumber);

    QList<Poppler::Link*> links = page->links();
    foreach(Poppler::Link* link, links) {
        QRectF linkarea = link->linkArea();
        QString area = QString::number(linkarea.left())
                + "," + QString::number(linkarea.top())
                + "," + QString::number(linkarea.right())
                + "," + QString::number(linkarea.bottom());

        if (link->linkType() == Poppler::Link::Browse) {
            // url
            Poppler::LinkBrowse* browseLink = static_cast<Poppler::LinkBrowse*>(link);
            s.append( area + ",url," + browseLink->url() + "\n");
        }
        else if (link->linkType() == Poppler::Link::Goto) {
            // internal link
            Poppler::LinkGoto* gotoLink = static_cast<Poppler::LinkGoto*>(link);
            s.append(area + ",page," + gotoLink->destination().pageNumber() + "\n");
        }
    }

    return answer;
}


void PdfServerThread::dpi(qreal &dpiX, qreal &dpiY, int zoomlevel)
{
    dpiX = 72;
    dpiY = 72;
#ifdef Q_WS_X11
    dpiX = QX11Info::appDpiX();
    dpiY = QX11Info::appDpiY();
#else
    QDesktopWidget *w = QApplication::desktop();
    if (w) {
        dpiX = w->logicalDpiX();
        dpiY = w->logicalDpiY();
    }
#endif

    dpiX = dpiX * zoomlevel;
    dpiY = dpiY * zoomlevel;

}
