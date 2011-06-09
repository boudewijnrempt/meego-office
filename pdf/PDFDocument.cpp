#include "PDFDocument.h"

#include <QtCore/QDebug>
#include <QtCore/QTime>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

#include "PDFServerConfig.h"
#include "PDFProcessManager.h"
#include "PDFPage.h"

class PDFDocument::Private
{
public:
    Private(PDFDocument *qq)
      : q(qq),
        manager(0),
        memoryLimit(104857600), //100MiB
        currentMemoryUsed(0),
        minCachedPages(3),
        pageCount(0),
        pageLayout(-1),
        width(0.0),
        height(0.0)
    { }
    
    void openRequestFinished();
    //void updateCache();

    PDFDocument *q;
    
    QUrl path;
    QNetworkAccessManager *manager;

    int memoryLimit;
    int currentMemoryUsed;
    int minCachedPages;

    int pageCount;
    int pageLayout;
    qreal width;
    qreal height;

    QSizeF documentSize;

    QHash<QString, QString> metaData;
    QList<PDFPage*> pages;
    QList<PDFPage*> loadedPages;

    static const QString serverPath;
};

const QString PDFDocument::Private::serverPath = QString("http://localhost:") + PDF_SERVER_PORT + "/";

PDFDocument::PDFDocument(QObject* parent, const QUrl &path)
    : QObject(parent), d(new Private(this))
{
    d->path = path;
    d->manager = new QNetworkAccessManager(this);
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

QSizeF PDFDocument::documentSize()
{
    return d->documentSize;
}

PDFPage* PDFDocument::page ( int pageNumber )
{
    if(pageNumber > 0 && pageNumber < d->pageCount) {
        return d->pages.at(pageNumber);
    }

    return 0;
}

QList< PDFPage* > PDFDocument::allPages()
{
    return d->pages;
}

QList< PDFPage* > PDFDocument::visiblePages ( const QRectF& viewRect )
{
    QList<PDFPage*> pages;
    foreach(PDFPage *page, d->pages) {
        if(viewRect.intersects(page->boundingRect())) {
            pages.append(page);
        }
    }
    return pages;
}

QNetworkAccessManager* PDFDocument::networkManager()
{
    return d->manager;
}

QNetworkRequest PDFDocument::buildRequest( const QString& command, const QString& arguments )
{
    return QNetworkRequest(QUrl(d->serverPath + command + "?file=" + d->path.toLocalFile() + (arguments.isEmpty() ? "" : "&" + arguments)));
}

void PDFDocument::open()
{
    PDFProcessManager::instance()->ensureRunning();

    connect(d->manager->get(buildRequest("open")), SIGNAL(finished()), SLOT(openRequestFinished()));
}

void PDFDocument::setPath ( const QUrl& path )
{
    d->path = path;
}

void PDFDocument::setMemoryLimit ( int bytes )
{
    d->memoryLimit = bytes;
}

void PDFDocument::setMinimumCachedPages ( int pages )
{
    d->minCachedPages = pages;
}

void PDFDocument::updateCache()
{
    QMap<QTime, PDFPage*> sortedPages;
    foreach(PDFPage* page, d->pages) {
        sortedPages.insert(page->lastVisibleTime(), page);
    }

    QList<PDFPage*> notUnloadPages;
    int remainingMemory = d->memoryLimit;
    int numPages = 0;
    QMap<QTime, PDFPage*>::iterator itr;
    for(itr = sortedPages.begin(); itr != sortedPages.end(); ++itr) {
        remainingMemory -= itr.value()->byteSize();
        numPages++;
        if(remainingMemory > 0 || numPages < d->minCachedPages) {
            notUnloadPages.append(itr.value());
        }
    }

    foreach(PDFPage *page, d->pages) {
        if(!notUnloadPages.contains(page)) {
            page->unload();
        }
    }
}

void PDFDocument::Private::openRequestFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(q->sender());
    if(!reply || reply->error() != QNetworkReply::NoError) {
        return;
    }

    pageCount = reply->rawHeader("X-PDF-NumberOfPages").toInt();
    pageLayout = reply->rawHeader("X-PDF-PageLayout").toInt();

    documentSize.setWidth(reply->rawHeader("X-PDF-Width").toFloat());
    documentSize.setHeight((reply->rawHeader("X-PDF-Height").toFloat() + 10) * pageCount);
    emit q->documentSizeChanged(documentSize);

    reply->close();

    for(int i = 0; i < pageCount; ++i) {
        PDFPage *newPage = new PDFPage(q, i);
        newPage->load();
        pages.append(newPage);
    }

    emit q->opened();
}

#include "PDFDocument.moc"
