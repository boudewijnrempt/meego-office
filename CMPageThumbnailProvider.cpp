#include "CMPageThumbnailProvider.h"
#include "CMCanvasControllerDeclarative.h"
#include "stage/CMStageCanvas.h"
#include "tables/CMTablesCanvas.h"
#include "words/CMWordsCanvas.h"
#include <QPainter>

class CMPageThumbnailProvider::Private
{
public:
    Private() : controller(0) { }
    ~Private() { }
    
    CMCanvasControllerDeclarative* controller;
};

CMPageThumbnailProvider::CMPageThumbnailProvider()
    : QDeclarativeImageProvider(QDeclarativeImageProvider::Image)
    , d(new Private)
{

}

CMPageThumbnailProvider::~CMPageThumbnailProvider()
{
    delete(d);
}

QImage CMPageThumbnailProvider::requestImage(const QString& id, QSize* size, const QSize& requestedSize)
{
    CMStageCanvas* stageCanvas = qobject_cast<CMStageCanvas*>(d->controller);
    CMTablesCanvas* tablesCanvas = qobject_cast<CMTablesCanvas*>(d->controller);
    CMWordsCanvas* wordsCanvas = qobject_cast<CMWordsCanvas*>(d->controller);
    
    QString pageNumber = id.section('/', 1);
    if(stageCanvas) {
        QImage tmp = QImage( QSize(64,48), QImage::Format_ARGB32 );
        tmp.fill( QColor("silver").rgb() );
        QPainter painter(&tmp);
        painter.setPen(Qt::gray);
        painter.setFont(QFont("sans-serif", 8));
        painter.drawText(tmp.rect(), Qt::AlignCenter, QString("Preview of\nSlide %1").arg(pageNumber));
        painter.end();
        return tmp;
    }
    else if(tablesCanvas) {
        QImage tmp = QImage( QSize(64,64), QImage::Format_ARGB32 );
        tmp.fill( QColor("silver").rgb() );
        QPainter painter(&tmp);
        painter.setPen(Qt::gray);
        painter.setFont(QFont("sans-serif", 8));
        painter.drawText(tmp.rect(), Qt::AlignCenter, QString("Preview of\nSheet %1").arg(pageNumber));
        painter.end();
        return tmp;
    }
    else if(wordsCanvas) {
        QImage tmp = QImage( QSize(48,64), QImage::Format_ARGB32 );
        tmp.fill( QColor("silver").rgb() );
        QPainter painter(&tmp);
        painter.setPen(Qt::gray);
        painter.setFont(QFont("sans-serif", 8));
        painter.drawText(tmp.rect(), Qt::AlignCenter, QString("Preview of\nPage %1").arg(pageNumber));
        painter.end();
        return tmp;
    }

    QImage tmp = QImage( QSize(64,64), QImage::Format_ARGB32 );
    tmp.fill( QColor("silver").rgb() );
    QPainter painter(&tmp);
    painter.setPen(Qt::gray);
    painter.setFont(QFont("sans-serif", 8));
    painter.drawText(tmp.rect(), Qt::AlignCenter, "Unknown\ncanvas\ntype");
    painter.end();
    return tmp;
}

void CMPageThumbnailProvider::documentChanged(QVariant newCanvasController)
{
    CMCanvasControllerDeclarative* newController = qobject_cast<CMCanvasControllerDeclarative*>(newCanvasController.value<QObject*>());
    if(newController)
    {
        d->controller = newController;
    }
}

#include "CMPageThumbnailProvider.moc"
