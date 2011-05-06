#include "CMIconImageProvider.h"

#include <QtDeclarative/QDeclarativeEngine>
#include <KDE/KIcon>
#include <KDE/KMimeType>

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

    QString requestedIcon = id;
    if(id.contains("by-path")) {
        requestedIcon = KMimeType::iconNameForUrl(KUrl(id.right(id.size() - 8)));
    }
    KIcon icon(requestedIcon);
    
    if(size) {
        *size = QSize(width, height);
    }
    
    return icon.pixmap(*size);
}
