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
    Q_SLOT void documentChanged(QVariant newCanvasController);
private:
    class Private;
    Private* d;
};

#endif // CMPAGETHUMBNAILPROVIDER_H
