#include "PDFPage.h"

#include <QtCore/QDebug>
#include <QtCore/QDateTime>
#include <QtGui/QImage>
#include <QtGui/QPainter>
#include <QtGui/QApplication>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

#include "PDFDocument.h"
#include <QPalette>

class PDFPage::Private
{
public:
    Private() : loaded(false) { }
    void requestFinished(QNetworkReply *reply);
    
    PDFDocument *document;
    QNetworkAccessManager *manager;
    int pageNumber;

    qreal documentPosition;
    qreal width;
    qreal height;
    bool loaded;

    QTime lastVisibleTime;

    QHash<QString, QImage> images;
    QHash<QString, QNetworkReply*> requests;
    QList< QRectF > highlights;
    QRectF currentHighlight;
};

PDFPage::PDFPage (PDFDocument *document, int pageNumber, qreal width, qreal height)
    : QObject(document), d(new Private)
{
    d->document = document;
    d->manager = document->networkManager();
    connect(d->manager, SIGNAL(finished(QNetworkReply*)), SLOT(requestFinished(QNetworkReply*)));
    d->pageNumber = pageNumber;

    d->width = width;
    d->height = height;
}

PDFPage::~PDFPage()
{

}

PDFDocument* PDFPage::document()
{
    return d->document;
}

qreal PDFPage::positionInDocument()
{
    return d->documentPosition;
}

qreal PDFPage::height()
{
    return d->height;
}

qreal PDFPage::width()
{
    return d->width;
}

QRectF PDFPage::boundingRect()
{
    return QRectF(0, d->documentPosition, d->width, d->height);
}

int PDFPage::pageNumber()
{
    return d->pageNumber;
}

QList< QRectF > PDFPage::highlights()
{
    return d->highlights;
}

int PDFPage::byteSize()
{
    int size = 0;
    QHash<QString, QImage>::const_iterator itr;
    for(itr = d->images.constBegin(); itr != d->images.constEnd(); ++itr) {
        size += itr.value().byteCount();
    }
    return size;
}

QImage PDFPage::image ( int width, int height )
{
    QString id = QString("%1x%2").arg(width).arg(height);

    if(d->images.contains(id)) {
        return d->images.value(id);
    } else {
        if(!d->requests.contains(id)) {
            d->requests.insert(id, d->manager->get(d->document->buildRequest("image", QString("page=%1&width=%2&height=%3").arg(d->pageNumber).arg(width).arg(height))) );
        }
        
        if(d->images.size() > 0) {
            return d->images.begin().value();
        }
    }

    return QImage();
}

QTime PDFPage::lastVisibleTime()
{
    return d->lastVisibleTime;
}

void PDFPage::load()
{
    if(!d->loaded && !d->requests.contains("load")) {
        d->requests.insert("load", d->manager->get(d->document->buildRequest("page", QString("page=%1").arg(d->pageNumber))) );
    }
}

void PDFPage::unload()
{
    foreach(QNetworkReply *reply, d->requests) {
        reply->close();
        reply->deleteLater();
    }
    
    d->requests.clear();
    d->images.clear();
    d->loaded = false;
}

void PDFPage::setPositionInDocument ( qreal position )
{
    d->documentPosition = position;
}

void PDFPage::setHighlights ( const QList< QRectF >& highlights )
{
    d->highlights = highlights;
}

void PDFPage::setCurrentHighlight ( const QRectF& highlight )
{
    d->currentHighlight = highlight;
}

void PDFPage::paint ( QPainter* painter, const QRectF& target )
{
    if(!d->loaded) {
        load();
    }
    
    d->lastVisibleTime = QTime::currentTime();
    
    painter->setBrush(QBrush(Qt::black));
    painter->setOpacity(0.2);
    painter->drawRect(target.translated(5, 5));
    painter->setOpacity(0.4);
    painter->drawRect(target.translated(4, 4));
    painter->setOpacity(0.6);
    painter->drawRect(target.translated(3, 3));
    painter->setOpacity(0.8);
    painter->drawRect(target.translated(2, 2));
    painter->setOpacity(1.0);
    painter->drawRect(target.translated(1, 1));
    painter->setBrush(QBrush(Qt::white));
    painter->drawRect(target);
    painter->drawImage(target, image(target.width(), target.height()));

    painter->setBrush(QBrush(Qt::yellow));
    painter->setPen(Qt::NoPen);
    painter->setOpacity(0.5);
    foreach(const QRectF &highlight, d->highlights) {
        if(highlight != d->currentHighlight) {
            painter->drawRect(highlight.translated(0, d->documentPosition));
        }
    }

    if(d->currentHighlight.isValid()) {
        painter->setBrush(QBrush(QApplication::palette().highlight()));
        painter->drawRect(d->currentHighlight.translated(0, d->documentPosition));
    }
}

void PDFPage::Private::requestFinished ( QNetworkReply* reply )
{
    QString command = reply->request().url().path();
    if(command != "/page" && command != "/image") {
        return;
    }
    
    if(reply->error() != QNetworkReply::NoError) {
        qDebug() << "Error retrieving page:" << reply->errorString();
        loaded = false;
        return;
    }

    if(reply->rawHeader("X-PDF-PageNumber").toInt() != pageNumber) {
        return;
    }

    if(command == "/page") {
        width = reply->rawHeader("X-PDF-PageWidth").toFloat();
        height = reply->rawHeader("X-PDF-PageHeight").toFloat();
        loaded = true;
    } else {
        QImage image;
        image.loadFromData(reply->readAll());
        if(!image.isNull()) {
            images.insert(QString("%1x%2").arg(image.width()).arg(image.height()), image);
        }
    }

    requests.remove(requests.key(reply));
    reply->deleteLater();
}

#include "PDFPage.moc"
