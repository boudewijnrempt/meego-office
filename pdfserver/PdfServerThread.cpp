#include "PdfServerThread.h"
#include "PdfDocument.h"
#include "PdfDocumentCache.h"
#include "PdfReply.h"

#include <QDebug>
#include <QRegExp>
#include <QStringList>
#include <QTextStream>
#include <QByteArray>
#include <QImage>
#include <QBuffer>
#include <QUrl>

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

    PdfReply reply;

    // first line is GET
    if (socket.canReadLine()) {
        QString line = socket.readLine();
        QStringList tokens = line.split(QRegExp("[ \r\n][ \r\n]*"));
        if (tokens.length() < 2 || tokens[0] != "GET") {
            reply.setStatus(PdfReply::Error_Status);
            reply.setErrorString("Not a GET request");
        }
        
        QUrl url(tokens[1]);
        if (!url.isValid() || url.isEmpty() || !url.hasQuery()) {
            reply.setStatus(PdfReply::Error_Status);
            reply.setErrorString("Incomplete query");
        }

        QString file = url.queryItemValue("file");
        if(file.isEmpty()) {
            reply.setStatus(PdfReply::Error_Status);
            reply.setErrorString("Incomplete query");
        }

        if(reply.status() == PdfReply::Undefined_Status) {
            QString command = url.path();
            if(command == "/open") {
                open(url, reply);
            } else if(command == "/getpage") {
                getpage(url, reply);
            } else if(command == "/thumbnail") {
                thumbnail(url, reply);
            } else if(command == "/search") {
                search(url, reply);
            } else if(command == "/text") {
                text(url, reply);
            } else if(command == "/links") {
                links(url, reply);
            } else {
                reply.setStatus(PdfReply::NotFound_Status);
                reply.setErrorString("Unknown command");
            }
        }
    }
    else {
        reply.setStatus(PdfReply::InternalError_Status);
        reply.setErrorString("Read Error");
    }

    int data = reply.write(socket);
    if(data == -1) {
        qWarning("Unable to write data to socket!");
        socket.close();
    }

    while(socket.bytesToWrite() > 0) {
        socket.waitForBytesWritten();
    }
    socket.close();
}


void PdfServerThread::open( const QUrl& url, PdfReply& reply)
{
    QByteArray answer;
    QString file = url.queryItemValue("file");

    if(file.isEmpty()) {
        reply.setStatus(PdfReply::NotFound_Status);
        reply.setErrorString("Not found");
        return;
    }

    PdfDocument *doc = m_documentCache->document(file);
    if (!doc || !doc->isValid()) {
        reply.setStatus(PdfReply::InternalError_Status);
        reply.setErrorString("Could not open file");
        return;
    }

    qreal dpiX, dpiY;
    dpi(dpiX, dpiY, 1.0);

    reply.setProperty("File", file);
    reply.setProperty("NumberOfPages", QString::number(doc->numberOfPages()));
    reply.setProperty("PageLayout", QString::number(doc->pageLayout()));
    reply.setProperty("Width", QString::number(doc->page(0)->pageSizeF().width()));
    reply.setProperty("Height", QString::number(doc->page(0)->pageSizeF().height()));

    QMap<QString, QString> infomap = doc->infoMap();
    foreach(const QString key, infomap.keys()) {
        reply.setProperty(key, infomap.value(key));
    }

    reply.setStatus(PdfReply::OK_Status);
    reply.setData("");
}

void PdfServerThread::getpage( const QUrl& url, PdfReply& reply)
{
    QByteArray answer;

    QString file = url.queryItemValue("file");
    bool pageOk = false;
    int pageNumber = url.queryItemValue("page").toInt(&pageOk);
    bool zoomOk = false;
    qreal zoom = url.queryItemValue("zoom").toDouble(&zoomOk);

    if(file.isEmpty() || !pageOk || !zoomOk) {
        reply.setStatus(PdfReply::NotFound_Status);
        reply.setErrorString("Not found");
        return;
    }

    PdfDocument *doc = m_documentCache->document(file);
    if (!doc || !doc->isValid()) {
        reply.setStatus(PdfReply::InternalError_Status);
        reply.setErrorString("Could not open file");
        return;
    }

    Poppler::Page *page = doc->page(pageNumber);
    if (!page) {
        reply.setStatus(PdfReply::NotFound_Status);
        reply.setErrorString("Page not found");
        return;
    }

    qreal dpiX, dpiY;
    dpi(dpiX, dpiY, zoom);

    // XXX: rendering quality isn't what it should be. Probably some error here
    QImage img = page->renderToImage(dpiX, dpiY);
    QByteArray imageBytes;
    QBuffer buf(&imageBytes);
    buf.open(QIODevice::WriteOnly | QIODevice::Append);
    img.save(&buf, "PNG");

    reply.setProperty("File", file);
    reply.setProperty("PageNumber", QString::number(pageNumber));
    reply.setProperty("ZoomLevel", QString::number(zoom));
    reply.setProperty("Orientation", QString::number(page->orientation()));

    reply.setData(imageBytes);
    reply.setContentType(PdfReply::Image_ContentType);
    reply.setStatus(PdfReply::OK_Status);
}

void PdfServerThread::thumbnail( const QUrl& url, PdfReply& reply)
{
    QByteArray answer;

    QString file = url.queryItemValue("file");
    bool pageOk = false;
    int pageNumber = url.queryItemValue("page").toInt(&pageOk);
    bool widthOk = false;
    int width = url.queryItemValue("width").toInt(&widthOk);
    bool heightOk = false;
    int height = url.queryItemValue("height").toInt(&heightOk);

    if(file.isEmpty() || !pageOk || !widthOk || !heightOk) {
        reply.setStatus(PdfReply::NotFound_Status);
        reply.setErrorString("Not found");
        return;
    }

    PdfDocument *doc = m_documentCache->document(file);
    if (!doc || !doc->isValid()) {
        reply.setStatus(PdfReply::InternalError_Status);
        reply.setErrorString("Could not open file");
        return;
    }

    qDebug() << pageNumber;
    Poppler::Page *page = doc->page(pageNumber);
    if (!page) {
        reply.setStatus(PdfReply::NotFound_Status);
        reply.setErrorString("Page not found");
        return;
    }

    QSize thumbsize(width, height);
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

    reply.setProperty("File", file);
    reply.setProperty("PageNumber", QString::number(pageNumber));
    reply.setProperty("Width", QString::number(width));
    reply.setProperty("Height", QString::number(height));

    reply.setData(imageBytes);
    reply.setContentType(PdfReply::Image_ContentType);
    reply.setStatus(PdfReply::OK_Status);
}

void PdfServerThread::search( const QUrl& url, PdfReply& reply)
{
    QByteArray answer;

    QString file = url.queryItemValue("file");
    bool pageOk = false;
    int pageNumber = url.queryItemValue("page").toInt(&pageOk);
    QString pattern = url.queryItemValue("pattern");

    if(file.isEmpty() || !pageOk || pattern.isEmpty()) {
        reply.setStatus(PdfReply::NotFound_Status);
        reply.setErrorString("Not found");
        return;
    }

    PdfDocument *doc = m_documentCache->document(file);
    if (!doc || !doc->isValid()) {
        reply.setStatus(PdfReply::InternalError_Status);
        reply.setErrorString("Could not open file");
        return;
    }

    Poppler::Page *page = doc->page(pageNumber);
    if (!page) {
        reply.setStatus(PdfReply::NotFound_Status);
        reply.setErrorString("Page not found");
        return;
    }

    reply.setProperty("File", file);
    reply.setProperty("PageNumber", QString::number(pageNumber));
    reply.setProperty("SearchPattern", pattern);

    bool found = true;
    double top, bottom, left, right = 0.0;

    QString s;
    while(found) {
        found = page->search(pattern, left, top, right, bottom,
                             Poppler::Page::NextResult, Poppler::Page::CaseInsensitive);
        if (found) {
            QString result("%1,%2,%3,%4\n");
            s.append(result.arg(left).arg(top).arg(right).arg(bottom));
        }
    }

    reply.setData(s.toUtf8());
    reply.setStatus(PdfReply::OK_Status);
}

void PdfServerThread::text( const QUrl& url, PdfReply& reply)
{
    QByteArray answer;

    QString file = url.queryItemValue("file");
    bool pageOk = false;
    int pageNumber = url.queryItemValue("page").toInt(&pageOk);
    bool leftOk = false;
    float left = url.queryItemValue("left").toFloat(&leftOk);
    bool topOk = false;
    float top = url.queryItemValue("top").toFloat(&topOk);
    bool rightOk = false;
    float right = url.queryItemValue("right").toFloat(&rightOk);
    bool bottomOk = false;
    float bottom = url.queryItemValue("bottom").toFloat(&bottomOk);

    if(file.isEmpty() || !pageOk || !leftOk || !topOk || !rightOk || !bottomOk) {
        reply.setStatus(PdfReply::NotFound_Status);
        reply.setErrorString("Not found");
        return;
    }
    
    PdfDocument *doc = m_documentCache->document(file);
    if (!doc || !doc->isValid()) {
        reply.setStatus(PdfReply::InternalError_Status);
        reply.setErrorString("Could not open file");
        return;
    }

    Poppler::Page *page = doc->page(pageNumber);
    if (!page) {
        reply.setStatus(PdfReply::NotFound_Status);
        reply.setErrorString("Page not found");
        return;
    }

    QRectF textRect = QRectF(QPointF(left, top), QPointF(right, bottom));
    QString text = page->text(textRect);

    reply.setProperty("File", file);
    reply.setProperty("PageNumber", QString::number(pageNumber));
    reply.setProperty("LeftTop" , QString("%1,%2").arg(left).arg(top));
    reply.setProperty("RightBottom", QString("%1,%2").arg(right).arg(bottom));

    reply.setData(text.toUtf8());
    reply.setStatus(PdfReply::OK_Status);
}

void PdfServerThread::links( const QUrl& url, PdfReply& reply)
{
    QByteArray answer;

    QString file = url.queryItemValue("file");
    bool pageOk = false;
    int pageNumber = url.queryItemValue("page").toInt(&pageOk);

    if(file.isEmpty() || !pageOk) {
        reply.setStatus(PdfReply::NotFound_Status);
        reply.setErrorString("Not found");
        return;
    }

    PdfDocument *doc = m_documentCache->document(file);
    if (!doc || !doc->isValid()) {
        reply.setStatus(PdfReply::InternalError_Status);
        reply.setErrorString("Could not open file");
        return;
    }

    Poppler::Page *page = doc->page(pageNumber);
    if (!page) {
        reply.setStatus(PdfReply::NotFound_Status);
        reply.setErrorString("Page not found");
        return;
    }

    QList<Poppler::Link*> links = page->links();

    reply.setProperty("File", file);
    reply.setProperty("PageNumber", QString::number(pageNumber));
    reply.setProperty("NumberOfLinks", QString::number(links.size()));

    QString s;
    foreach(Poppler::Link* link, links) {
        QRectF linkarea = link->linkArea();
        QString area = QString::number(linkarea.left())
                + "," + QString::number(linkarea.top())
                + "," + QString::number(linkarea.right())
                + "," + QString::number(linkarea.bottom());

        if (link->linkType() == Poppler::Link::Browse) {
            // url
            Poppler::LinkBrowse* browseLink = static_cast<Poppler::LinkBrowse*>(link);
            s = s.append( area + ",url," + browseLink->url() + "\n");
        }
        else if (link->linkType() == Poppler::Link::Goto) {
            // internal link
            Poppler::LinkGoto* gotoLink = static_cast<Poppler::LinkGoto*>(link);
            s = s.append(area + ",page," + gotoLink->destination().pageNumber() + "\n");
        }
    }

    reply.setData(s.toUtf8());
    reply.setStatus(PdfReply::OK_Status);
}


void PdfServerThread::dpi(qreal &dpiX, qreal &dpiY, qreal zoomlevel)
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
