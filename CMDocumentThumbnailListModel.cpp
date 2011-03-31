#include "CMDocumentThumbnailListModel.h"

#include <KoDocument.h>
#include <KoDocumentInfo.h>
#include <QImage>
#include <tables/DocBase.h>
#include <tables/Map.h>
#include <tables/Sheet.h>

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
            if(qobject_cast<Calligra::Tables::DocBase*>(d->document)) {
                var = QVariant::fromValue<QString>( qobject_cast<Calligra::Tables::DocBase*>(d->document)->map()->sheet(index.row())->sheetName() );
            }
            else {
                var = QVariant::fromValue<QString>( QString() );
            }
            break;
        case PageNumberRole:
            // This is the page number
            var = QVariant::fromValue<QString>( QString::number( index.row() + 1 ) );
            break;
        case PageThumbnailRole:
            // This is the thumbnail
            // THIS IS HACKERY FOR NOW! Just doing this to have a nice unique one for the time being
            // THIS WILL NOT BE STAYING IN!
            QString someIDWhichDefinesTheDocumentUniquely = QString::number(reinterpret_cast<int64_t>(d->document));
            var = QVariant::fromValue<QString>( QString("image://pagethumbnails/%1/%2").arg(someIDWhichDefinesTheDocumentUniquely).arg(index.row() + 1) );
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
    {
        if(qobject_cast<Calligra::Tables::DocBase*>(d->document)) {
            return qobject_cast<Calligra::Tables::DocBase*>(d->document)->map()->count();
        }
        return d->document->pageCount();
    }

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

bool CMDocumentThumbnailListModel::hasOwnPageNumbering() const
{
    bool result = false;
    if(d->document)
    {
        if(qobject_cast<Calligra::Tables::DocBase*>(d->document)) {
            result = true;
        }
        else {
            result = false;
        }
    }
    return result;
}


#include "CMDocumentThumbnailListModel.moc"
