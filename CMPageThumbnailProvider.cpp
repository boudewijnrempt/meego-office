#include "CMPageThumbnailProvider.h"
#include "CMCanvasControllerDeclarative.h"
#include "stage/CMStageCanvas.h"
#include "tables/CMTablesCanvas.h"
#include "words/CMWordsCanvas.h"
#include <QPainter>
#include <part/KWPage.h>
#include <part/KWCanvas.h>

class CMPageThumbnailProvider::Private
{
public:
    Private() { }
    ~Private() { }
    
    QWeakPointer<CMCanvasControllerDeclarative> controller;
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
    if(d->controller.isNull())
        return QImage();
    
    CMStageCanvas* stageCanvas = qobject_cast<CMStageCanvas*>(d->controller.data());
    CMTablesCanvas* tablesCanvas = qobject_cast<CMTablesCanvas*>(d->controller.data());
    CMWordsCanvas* wordsCanvas = qobject_cast<CMWordsCanvas*>(d->controller.data());
    
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
        return qobject_cast<KWDocument*>(wordsCanvas->doc())->pageManager()->page(pageNumber.toInt()).thumbnail(requestedSize, wordsCanvas->canvas()->shapeManager());
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
        d->controller = QWeakPointer<CMCanvasControllerDeclarative>(newController);
    }
}

#include "CMPageThumbnailProvider.moc"
