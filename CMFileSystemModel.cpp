#include "CMFileSystemModel.h"
#include <KDE/KDirLister>
#include <KDE/KDirModel>

class CMFileSystemModel::Private
{
public:
    KDirModel* dirModel;
};

CMFileSystemModel::CMFileSystemModel(QObject* parent)
    : KDirSortFilterProxyModel(parent), d(new Private)
{
    d->dirModel = new KDirModel(this);
    setSourceModel(d->dirModel);
    setSortFoldersFirst(true);
    
    QHash<int, QByteArray> roles;
    roles.insert(FileNameRole, "fileName");
    roles.insert(FilePathRole, "filePath");
    roles.insert(FileIconRole, "fileIcon");
    roles.insert(FileTypeRole, "fileType");
    setRoleNames(roles);
}

CMFileSystemModel::~CMFileSystemModel()
{
    delete d;
}

QVariant CMFileSystemModel::data(const QModelIndex& index, int role) const
{
    if(index.isValid()) {
        KFileItem item = d->dirModel->itemForIndex(mapToSource(index));
        if(!item.isNull()) {
            switch(role) {
                case FileNameRole:
                    return item.text();
                    break;
                case FilePathRole:
                    return item.url().toLocalFile();
                    break;
                case FileIconRole:
                    return item.iconName();
                    break;
                case FileTypeRole:
                    return item.mimetype();
                    break;
            }
        }
    }
    return KDirSortFilterProxyModel::data(index, role);
}

void CMFileSystemModel::classBegin()
{

}

void CMFileSystemModel::componentComplete()
{
    setRootPath(QDir::homePath());
}

QString CMFileSystemModel::rootPath()
{
    return d->dirModel->dirLister()->url().toLocalFile();
}

void CMFileSystemModel::setRootPath(const QString& path)
{
    d->dirModel->dirLister()->openUrl(KUrl::fromPath(path));
}

QString CMFileSystemModel::parentFolder()
{
    KUrl root = d->dirModel->dirLister()->url();
    root.cd("..");
    return root.toLocalFile();
}

QString CMFileSystemModel::filter()
{
    return d->dirModel->dirLister()->nameFilter();
}

void CMFileSystemModel::setFilter(const QString& filter)
{
    d->dirModel->dirLister()->setNameFilter(filter);
    d->dirModel->dirLister()->emitChanges();
}

