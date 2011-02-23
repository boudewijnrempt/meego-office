#include "CMIconImageProvider.h"

#include <QtDeclarative/QDeclarativeEngine>
#include <KDE/KIcon>

CMIconImageProvider::CMIconImageProvider()
    : QDeclarativeImageProvider(QDeclarativeImageProvider::Pixmap)
{

}

QPixmap CMIconImageProvider::requestPixmap( const QString &id, QSize *size, const QSize &requestedSize )
{
    int width = 64;
    int height = 64;

    if(requestedSize.width() > 0) {
        width = requestedSize.width();
    }

    if(requestedSize.height() > 0) {
        height = requestedSize.height();
    }

    KIcon icon(id);
    
    if(size) {
        *size = QSize(width, height);
    }
    
    return icon.pixmap(*size);
}
