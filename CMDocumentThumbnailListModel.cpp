#include "CMDocumentThumbnailListModel.h"

#include <KoDocument.h>
#include <KoDocumentInfo.h>
#include <QImage>

class CMDocumentThumbnailListModel::Private
{
public:
    Private()
        : document(0)
    {}
    ~Private() {}
    
    KoDocument* document;
};

CMDocumentThumbnailListModel::CMDocumentThumbnailListModel(QObject* parent)
    : QAbstractListModel(parent)
    , d(new Private())
{
    QHash<int, QByteArray> roles;
    roles[PageThumbnailRole] = "thumbnail";
    roles[PageNumberRole] = "pagenumber";
    roles[PageNameRole] = "pagename";
    setRoleNames(roles);
}

CMDocumentThumbnailListModel::~CMDocumentThumbnailListModel()
{
    delete(d);
}

QVariant CMDocumentThumbnailListModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return QVariant();

    QVariant var;
    switch(role)
    {
        case PageNameRole:
            // This is the page name, in case we've got custom page numbering
            // If we do not have custom page numbering in the document, just return an empty string
            var = QVariant::fromValue<QString>( QString() );
            break;
        case PageNumberRole:
            // This is the page number
            var = QVariant::fromValue<QString>( QString::number( index.row() + 1 ) );
            break;
        case PageThumbnailRole:
            // This is the thumbnail
            var = QVariant::fromValue<QImage>( QImage( QSize(64,64), QImage::Format_ARGB32 ) );
            break;
        default:
            // Nothing - just return an invalid variant
            break;
    }
    return var;
}

int CMDocumentThumbnailListModel::rowCount(const QModelIndex& parent) const
{
    // This is no tree, so any valid variant will have no children
    if(parent.isValid())
        return 0;

    if(d->document)
        return d->document->pageCount();

    // If we don't have a valid document, we'll also not have any pages
    return 0;
}

QVariant CMDocumentThumbnailListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QAbstractItemModel::headerData(section, orientation, role);
}

void CMDocumentThumbnailListModel::setDocument(QObject* doc)
{
    if(qobject_cast<KoDocument*>(doc))
        d->document = qobject_cast<KoDocument*>(doc);
    reset();
}

QObject* CMDocumentThumbnailListModel::document() const
{
    return d->document;
}


#include "CMDocumentThumbnailListModel.moc"
