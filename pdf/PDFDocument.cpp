#include "PDFDocument.h"

#include <QtCore/QDebug>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

#include "PDFServerConfig.h"
#include "PDFProcessManager.h"

class PDFDocument::Private
{
public:
    Private(PDFDocument *qq)
        : q(qq),
          manager(0),
          pageCount(0),
          pageLayout(-1),
          width(0.0),
          height(0.0)
    { }
    
    QUrl buildRequestUrl(const QString &command, const QString &arguments = QString());
    void requestFinished(QNetworkReply *reply);

    PDFDocument *q;
    
    QUrl path;
    QNetworkAccessManager *manager;

    int pageCount;
    int pageLayout;
    qreal width;
    qreal height;

    QSizeF documentSize;

    QHash<QString, QString> metaData;
    QHash<int, PDFPage*> pages;
    QHash<int, QNetworkReply*> requests;

    static const QString serverPath;
};

const QString PDFDocument::Private::serverPath = QString("http://localhost:") + PDF_SERVER_PORT + "/";

PDFDocument::PDFDocument(QObject* parent, const QUrl &path)
    : QObject(parent), d(new Private(this))
{
    d->path = path;
    d->manager = new QNetworkAccessManager(this);
    connect(d->manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(requestFinished(QNetworkReply*)));
}

PDFDocument::~PDFDocument()
{
    qDeleteAll(d->pages);
}

QUrl PDFDocument::path()
{
    return d->path;
}

int PDFDocument::pageCount()
{
    return d->pageCount;
}

int PDFDocument::pageLayout()
{
    return d->pageLayout;
}

qreal PDFDocument::height()
{
    return d->height;
}

qreal PDFDocument::width()
{
    return d->width;
}

QSizeF PDFDocument::documentSize()
{
    return d->documentSize;
}

PDFDocument::PDFPage* PDFDocument::page ( int pageNumber, bool requestIfNotAvailable )
{
    if(d->pages.contains(pageNumber)) {
        return d->pages.value(pageNumber);
    }

    if(requestIfNotAvailable) {
        requestPage(pageNumber);
    }

    return 0;
}

void PDFDocument::open()
{
    PDFProcessManager::instance()->ensureRunning();

    d->manager->get(QNetworkRequest(d->buildRequestUrl("open")));
}

void PDFDocument::requestPage ( int page, qreal zoom )
{
    if(page < 0 || page >= d->pageCount) {
        return;
    }

    if(!d->pages.contains(page) || !d->requests.contains(page)) {
        d->requests.insert(page, d->manager->get(QNetworkRequest(d->buildRequestUrl("getpage", QString("page=%1&zoom=%2").arg(page).arg(zoom)))));
    }
}

void PDFDocument::setPath ( const QUrl& path )
{
    d->path = path;
}

QUrl PDFDocument::Private::buildRequestUrl ( const QString& command, const QString& arguments )
{
    return QUrl(serverPath + command + "?file=" + path.toLocalFile() + (arguments.isEmpty() ? "" : "&" + arguments));
}

void PDFDocument::Private::requestFinished(QNetworkReply *reply)
{
    if(reply->error() != QNetworkReply::NoError) {
        qDebug() << "Error retrieving" << reply->request().url() << "Error:" << reply->errorString();
    }

    QString command = reply->request().url().path();
    
    if(command == "/open") {
        pageCount = reply->rawHeader("X-PDF-NumberOfPages").toInt();
        pageLayout = reply->rawHeader("X-PDF-PageLayout").toInt();

        reply->close();

        emit q->opened();
    } else if(command == "/getpage") {
        QByteArray data = reply->readAll();

        PDFPage *page = new PDFPage;
        page->pageNumber = reply->rawHeader("X-PDF-PageNumber").toInt();
        page->image.loadFromData(data, "PNG");
        page->orientation = reply->rawHeader("X-PDF-Orientation").toInt();
        page->width = page->image.width();
        page->height = page->image.height();

        if(!documentSize.isValid()) {
            documentSize.setWidth(page->width);
            documentSize.setHeight((page->height + 10) * pageCount);
            emit q->documentSizeChanged(documentSize);
        }

        pages.insert(page->pageNumber, page);
        requests.remove(page->pageNumber);
        
        emit q->newPage(page->pageNumber);
    }
}

#include "PDFDocument.moc"
