#ifndef CMPAGETHUMBNAILPROVIDER_H
#define CMPAGETHUMBNAILPROVIDER_H

#include <QDeclarativeImageProvider>
#include <QObject>


class CMPageThumbnailProvider : public QObject, public QDeclarativeImageProvider
{
Q_OBJECT
public:
    CMPageThumbnailProvider();
    virtual ~CMPageThumbnailProvider();
    virtual QImage requestImage(const QString& id, QSize* size, const QSize& requestedSize);
    void addThumbnail(QString id, QImage thumb);
private:
    class Private;
    Private* d;
};

#endif // CMPAGETHUMBNAILPROVIDER_H
