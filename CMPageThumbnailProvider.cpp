#include "CMPageThumbnailProvider.h"

CMPageThumbnailProvider::CMPageThumbnailProvider()
    : QDeclarativeImageProvider(QDeclarativeImageProvider::Image)
{

}


QImage CMPageThumbnailProvider::requestImage(const QString& id, QSize* size, const QSize& requestedSize)
{
    QImage tmp = QImage( QSize(64,64), QImage::Format_ARGB32 );
    tmp.fill( QColor("silver").rgb() );
    return tmp;
}


