#ifndef CMPAGETHUMBNAILPROVIDER_H
#define CMPAGETHUMBNAILPROVIDER_H

#include <QDeclarativeImageProvider>


class CMPageThumbnailProvider : public QDeclarativeImageProvider
{

public:
    CMPageThumbnailProvider();
    virtual QImage requestImage(const QString& id, QSize* size, const QSize& requestedSize);
};

#endif // CMPAGETHUMBNAILPROVIDER_H
