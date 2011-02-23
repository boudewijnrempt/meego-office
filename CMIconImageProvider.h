#ifndef CALLIGRAMOBILE_ICONIMAGEPROVIDER_H
#define CALLIGRAMOBILE_ICONIMAGEPROVIDER_H

#include <QtDeclarative/QDeclarativeImageProvider>
#include "calligramobile_export.h"

class CALLIGRAMOBILE_EXPORT CMIconImageProvider : public QDeclarativeImageProvider
{

public:
    CMIconImageProvider();
    
    QPixmap requestPixmap( const QString& id, QSize* size, const QSize& requestedSize );
};

#endif // CALLIGRAMOBILE_ICONIMAGEPROVIDER_H
