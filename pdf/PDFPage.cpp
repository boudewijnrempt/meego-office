#include "PDFPage.h"

#include <QtCore/QDebug>
#include <QtGui/QImage>
#include <QtGui/QPainter>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

#include "PDFDocument.h"
#include <QDateTime>

class PDFPage::Private
{
public:
    Private() : width(400), height(500) { }
    void requestFinished(QNetworkReply *reply);
    
    PDFDocument *document;
    QNetworkAccessManager *manager;
    int pageNumber;

    qreal documentPosition;
    qreal width;
    qreal height;

    QTime lastVisibleTime;

    QHash<QString, QImage> images;
};

PDFPage::PDFPage (PDFDocument *document, int pageNumber)
    : QObject(document), d(new Private)
{
    d->document = document;
    d->manager = document->networkManager();
    connect(d->manager, SIGNAL(finished(QNetworkReply*)), SLOT(requestFinished(QNetworkReply*)));
    d->pageNumber = pageNumber;
}

PDFPage::~PDFPage()
{

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
        d->manager->get(d->document->buildRequest("image", QString("page=%1&width=%2&height=%3").arg(d->pageNumber).arg(width).arg(height)));
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
    d->manager->get(d->document->buildRequest("page", QString("page=%1").arg(d->pageNumber)));
}

void PDFPage::unload()
{
    QImage thumb = d->images.value("40x50");
    d->images.clear();
    d->images.insert("40x50", thumb);
}

void PDFPage::setPositionInDocument ( qreal position )
{
    d->documentPosition = position;
}

void PDFPage::paint ( QPainter* painter, const QRectF& target )
{
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
}

void PDFPage::Private::requestFinished ( QNetworkReply* reply )
{
    if(reply->error() != QNetworkReply::NoError) {
        qDebug() << "Error retrieving page:" << reply->errorString();
        return;
    }

    if(reply->rawHeader("X-PDF-PageNumber").toInt() != pageNumber) {
        return;
    }

    if(reply->request().url().path() == "/page") {
        width = reply->rawHeader("X-PDF-PageWidth").toFloat();
        height = reply->rawHeader("X-PDF-PageHeight").toFloat();
        manager->get(document->buildRequest("image", QString("page=%1&width=%2&height=%3").arg(pageNumber).arg(int(width / 10)).arg(int(height / 10))));
    } else {
        QImage image;
        image.loadFromData(reply->readAll());

        if(!image.isNull()) {
            images.insert(QString("%1x%2").arg(image.width()).arg(image.height()), image);
        }
    }
}

#include "PDFPage.moc"
