#include "CMPageThumbnailProvider.h"
#include "CMCanvasControllerDeclarative.h"
#include "stage/CMStageCanvas.h"
#include "tables/CMTablesCanvas.h"
#include "words/CMWordsCanvas.h"
#include <QPainter>
#include <part/KWPage.h>
#include <part/KWCanvas.h>
#include <KoShapeManager.h>
#include <QApplication>

class CMPageThumbnailProvider::Private
{
public:
    Private() { }
    ~Private() { }
    
    QHash<QString,QImage> thumbnails;
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
    if(d->thumbnails.contains(id))
        return d->thumbnails[id];
    return QImage();
}

void CMPageThumbnailProvider::addThumbnail(QString id, QImage thumb)
{
    d->thumbnails[id] = thumb;
}

bool CMPageThumbnailProvider::hasThumbnail(QString id)
{
    return d->thumbnails.contains(id);
}

void doThatDebugThang(int level, QObject* object)
{
    if (object) {
        QByteArray buf;
        buf.fill(' ', level / 2 * 8);
        if (level % 2)
            buf += "    ";
        QString name = object->objectName();    
        QString flags = QLatin1String("");
        qDebug("%s%s::%s %s", (const char*)buf, object->metaObject()->className(), name.toLocal8Bit().data(),
            flags.toLatin1().data());
        QObjectList children = object->children();
        if (!children.isEmpty()) {
            for (int i = 0; i < children.size(); ++i)
                doThatDebugThang(level+1, children.at(i));
        }
    }
}

#include "CMPageThumbnailProvider.moc"
