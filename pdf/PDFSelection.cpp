#include "PDFSelection.h"

#include <QtCore/QDebug>
#include <QtGui/QApplication>
#include <QtGui/QClipboard>
#include <QtGui/QPainter>

#include "PDFDocument.h"
#include <QNetworkReply>
#include "PDFPage.h"

class PDFSelection::Private
{
public:
    Private(PDFSelection *qq)
        : q(qq),
          currentPage(0),
          textReply(0)
    { }
    
    void geometryChanged();
    void textRequestFinished();

    PDFSelection* q;
    PDFDocument *document;
    QNetworkReply *textReply;
    PDFPage *currentPage;
    QPoint documentOffset;
};

PDFSelection::PDFSelection ( PDFDocument *document, QGraphicsItem *parentItem, Qt::WindowFlags flags )
    : QGraphicsWidget(parentItem, flags), d(new Private(this))
{
    d->document = document;
    connect(this, SIGNAL(geometryChanged()), SLOT(geometryChanged()));
}

PDFSelection::~PDFSelection()
{

}

void PDFSelection::paint ( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget )
{
    QBrush blue(qApp->palette().highlight());
    painter->setPen(QPen(blue, 2));
    painter->setBrush(blue);
    painter->setOpacity(0.5);
    painter->drawRect(0, 0, geometry().width(), geometry().height());
}

void PDFSelection::copy()
{
    if(d->textReply) {
        d->textReply->close();
        delete d->textReply;
        d->textReply = 0;
    }

    PDFPage *page = d->currentPage;
    if(!page) {
        qDebug() << "Invalid page";
        return;
    }

    QRectF geom = geometry();
    geom.setLeft(geom.left() + d->documentOffset.x());
    geom.setRight(geom.right() + d->documentOffset.x());
    geom.setTop((geom.top() + d->documentOffset.y()) - page->positionInDocument() );
    geom.setBottom((geom.bottom() + d->documentOffset.y()) - page->positionInDocument() );
    QRectF sel(QPointF(geom.left() / page->width(), geom.top() / page->height()), QPointF(geom.right() / page->width(), geom.bottom() / page->height()));
        
    QString arguments = QString("page=%1&top=%2&right=%3&bottom=%4&left=%5").arg(page->pageNumber()).arg(sel.top()).arg(sel.right()).arg(sel.bottom()).arg(sel.left());
    qDebug() << arguments;
    d->textReply = d->document->networkManager()->get(d->document->buildRequest("text", arguments));
    connect(d->textReply, SIGNAL(finished()), SLOT(textRequestFinished()));
}

void PDFSelection::setCurrentPage ( PDFPage *page )
{
    d->currentPage = page;
}

void PDFSelection::setDocumentOffset ( QPoint offset )
{
    d->documentOffset = offset;
}

void PDFSelection::Private::geometryChanged()
{
    
}

void PDFSelection::Private::textRequestFinished()
{
    if(textReply->error() != QNetworkReply::NoError) {
        qDebug() << "Error in selection" << textReply->errorString();
        return;
    }

    qDebug() << textReply->rawHeader("X-PDF-PageNumber");
    qDebug() << textReply->rawHeader("X-PDF-LeftTop");
    qDebug() << textReply->rawHeader("X-PDF-RightBottom");

    QMimeData *mimeData = new QMimeData;
    mimeData->setText(textReply->readAll());
    qDebug() << mimeData->text();
    QApplication::clipboard()->setMimeData(mimeData);
}

#include "PDFSelection.moc"
