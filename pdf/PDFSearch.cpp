#include "PDFSearch.h"

#include <QtCore/QStringList>
#include <QtCore/QDebug>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

#include "PDFDocument.h"

class PDFSearch::Private
{
public:
    Private(PDFSearch *qq) : q(qq), document(0), page(0), lastReply(0), stopSearching(false) { }

    void searchFinished();

    PDFSearch* q;
    
    PDFDocument *document;
    int page;
    QString currentPattern;

    KoFindMatchList matches;
    QNetworkReply* lastReply;
    bool stopSearching;
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

void PDFSearch::finished()
{
    d->stopSearching = true;
    KoFindBase::finished();
}

void PDFSearch::replaceImplementation ( const KoFindMatch& match, const QVariant& value )
{
    Q_UNUSED(match)
    Q_UNUSED(value)
    //Intentionally does nothing.
}

void PDFSearch::findImplementation ( const QString& pattern, KoFindBase::KoFindMatchList& matchList )
{
    if(pattern != d->currentPattern) {
        //Reset the internal list of matches when we are changing what we search for.
        d->matches.clear();
    }

    if(d->matches.isEmpty()) {
        //Reset some of the internal variables. This will start a search from the first page again.
        d->currentPattern = pattern;
        d->page = 0;
        d->stopSearching = false;

        //We are not currently in the process of searching. So start a new search.
        if(!d->lastReply) {
            d->lastReply = d->document->networkManager()->get(d->document->buildRequest("search", QString("page=%1&pattern=%2").arg(d->page).arg(pattern)));
            connect(d->lastReply, SIGNAL(finished()), this, SLOT(searchFinished()));
        }
        
    } else {
        //We are currently busy with searching and do not need to restart the search. So just return our internal list of matches.
        matchList = d->matches;

        //And perform the highlighting.
        foreach(const KoFindMatch &match, matchList) {
            PDFPage *page = match.container().value<PDFPage*>();
            QList<QRectF> highlights = page->highlights();
            highlights.append(match.location().toRectF());
            page->setHighlights(highlights);
        }
    }
}

void PDFSearch::clearMatches()
{
    QList<PDFPage*> pages = d->document->allPages();
    foreach(PDFPage* page, pages) {
        page->setHighlights(QList<QRectF>());
    }
}

void PDFSearch::Private::searchFinished()
{
    QNetworkReply *reply = lastReply;

    //If we encountered an error, abort.
    if(reply->error() != QNetworkReply::NoError) {
        qDebug() << "Error in search:" << reply->errorString();
        return;
    }

    //Do not process anything unless we are certain the search data is correct.
    if(reply->rawHeader("X-PDF-SearchPattern") == currentPattern) {
        QStringList data = QString(reply->readAll()).split('\n');
        if(!data.isEmpty()) {
            PDFPage *pg = document->page(page);
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

                QRectF matchArea(QPointF(left, top), QPointF(right, bottom));
                match.setLocation(QVariant::fromValue<QRectF>(matchArea));
                matches.append(match);
            }
            emit q->searchUpdate();
        }

        page++;
    }

    if(page < document->pageCount() && !stopSearching) {
        lastReply = document->networkManager()->get(document->buildRequest("search", QString("page=%1&pattern=%2").arg(page).arg(currentPattern)));
        connect(lastReply, SIGNAL(finished()), q, SLOT(searchFinished()));
    } else {
        lastReply = 0;
    }

    reply->deleteLater();
}

#include "PDFSearch.moc"
