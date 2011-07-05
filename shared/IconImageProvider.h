#ifndef CALLIGRAMOBILE_ICONIMAGEPROVIDER_H
#define CALLIGRAMOBILE_ICONIMAGEPROVIDER_H

#include <QtDeclarative/QDeclarativeImageProvider>

class IconImageProvider : public QDeclarativeImageProvider
{

public:
    IconImageProvider();
    
    QPixmap requestPixmap( const QString& id, QSize* size, const QSize& requestedSize );
};

#endif // CALLIGRAMOBILE_ICONIMAGEPROVIDER_H
