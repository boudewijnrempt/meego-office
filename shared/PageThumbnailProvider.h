#ifndef CMPAGETHUMBNAILPROVIDER_H
#define CMPAGETHUMBNAILPROVIDER_H

#include <QDeclarativeImageProvider>
#include <QObject>

class PageThumbnailProvider : public QObject, public QDeclarativeImageProvider
{
    Q_OBJECT
public:
    PageThumbnailProvider();
    virtual ~PageThumbnailProvider();
    
    virtual QImage requestImage(const QString& id, QSize* size, const QSize& requestedSize);
    
    void addThumbnail(QString id, QImage thumb);
    bool hasThumbnail(QString id);
    
private:
    class Private;
    Private* d;
};

#endif // CMPAGETHUMBNAILPROVIDER_H
