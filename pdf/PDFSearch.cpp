#include "PDFSearch.h"

#include <QtCore/QStringList>
#include <QtCore/QDebug>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

#include "PDFDocument.h"

class PDFSearch::Private
{
public:
    Private(PDFSearch *qq) : q(qq), document(0), page(0), lastReply(0) { }

    void searchFinished();

    PDFSearch* q;
    
    PDFDocument *document;
    int page;
    QString currentPattern;

    KoFindMatchList matches;
    QNetworkReply* lastReply;
};

PDFSearch::PDFSearch ( PDFDocument* document, QObject* parent )
    : KoFindBase ( parent ), d(new Private(this))
{
    d->document = document;
}

PDFSearch::~PDFSearch()
{

}

QString PDFSearch::currentPattern()
{
    return d->currentPattern;
}

void PDFSearch::replaceImplementation ( const KoFindMatch& match, const QVariant& value )
{
    Q_UNUSED(match)
    Q_UNUSED(value)
    //Intentionally does nothing.
}

void PDFSearch::findImplementation ( const QString& pattern, KoFindBase::KoFindMatchList& matchList )
{
    qDebug() << "findImpl";
    if(pattern != d->currentPattern) {
        d->matches.clear();
        if(d->lastReply) {
            d->lastReply->close();
            d->lastReply->deleteLater();
        }
    }

    if(d->matches.isEmpty()) {
        d->currentPattern = pattern;
        d->lastReply = d->document->networkManager()->get(d->document->buildRequest("search", QString("page=%1&pattern=%2").arg(d->page).arg(pattern)));
        d->page = 0;
        connect(d->lastReply, SIGNAL(finished()), this, SLOT(searchFinished()));
    } else {
        matchList = d->matches;
    }
}

void PDFSearch::clearMatches()
{
    qDebug() << "clearMatches";
    QList<PDFPage*> pages = d->document->allPages();
    foreach(PDFPage* page, pages) {
        page->setHighlights(QList<QRectF>());
    }
}

void PDFSearch::Private::searchFinished()
{
    QNetworkReply *reply = lastReply;
    if(reply->error() != QNetworkReply::NoError) {
        return;
    }

    QStringList data = QString(reply->readAll()).split('\n');
    if(!data.isEmpty()) {
        PDFPage *pg = document->page(page);
        QList<QRectF> highlights = pg->highlights();
        
        foreach(const QString &matchString, data) {
            QStringList corners = matchString.split(',');
            if(corners.size() < 4) {
                continue;
            }

            KoFindMatch match;
            match.setContainer(QVariant::fromValue<PDFPage*>(pg));

            qreal left = corners.at(0).toFloat() * pg->width();
            qreal top = corners.at(1).toFloat() * pg->height();
            qreal right = corners.at(2).toFloat() * pg->width();
            qreal bottom = corners.at(3).toFloat() * pg->height();

            qDebug() << left << top << right << bottom;
            
            QRectF matchArea(QPointF(left, top), QPointF(right, bottom));
            match.setLocation(QVariant::fromValue<QRectF>(matchArea));
            highlights.append(matchArea);

            matches.append(match);
        }
        pg->setHighlights(highlights);
        emit q->searchUpdate();
    }

    page++;
    if(page < document->pageCount()) {
        lastReply = document->networkManager()->get(document->buildRequest("search", QString("page=%1&pattern=%2").arg(page).arg(currentPattern)));
        connect(lastReply, SIGNAL(finished()), q, SLOT(searchFinished()));
    } else {
        lastReply = 0;
    }

    reply->deleteLater();
}

#include "PDFSearch.moc"
