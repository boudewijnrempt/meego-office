#include "CMRecentFilesModel.h"

#include <QtCore/QDebug>
#include <KDE/KGlobal>
#include <KDE/KSharedConfig>
#include <KDE/KConfigGroup>

class CMRecentFilesModel::Private
{
public:
    QStringList data;
};

CMRecentFilesModel::CMRecentFilesModel(QObject* parent)
    : QStringListModel(parent), d(new Private)
{
    setStringList(d->data);
    QHash<int, QByteArray> roles;
    roles.insert(FileNameRole, "fileName");
    setRoleNames(roles);
}

CMRecentFilesModel::~CMRecentFilesModel()
{
    KConfigGroup group(KGlobal::config(), "Recent Files");
    int index = 0;
    foreach(QString string, d->data) {
        group.writeEntry(QString().setNum(index), string);
        index++;
    }
    group.sync();
    delete d;
}

QVariant CMRecentFilesModel::data(const QModelIndex& index, int role) const
{
    if(index.isValid() && role == FileNameRole)
    {
        return d->data.at(index.row());
    }
    return QStringListModel::data(index, role);
}

void CMRecentFilesModel::classBegin()
{

}

void CMRecentFilesModel::componentComplete()
{
    KConfigGroup group(KGlobal::config(), "Recent Files");
    
    QStringList keys = group.keyList();
    for(int i = keys.count() - 1; i >= 0; --i) {
        d->data.prepend(group.readEntry(keys[i]));
        insertRows(0, 1);
    }
}

void CMRecentFilesModel::addFile(const QString& file)
{
    if(!d->data.contains(file)) {
        d->data.prepend(file);
        insertRows(0, 1);
        if(d->data.size() > 10) {
            removeRows(10, d->data.size() - 10);
            d->data.erase(d->data.begin() + 10, d->data.end());
        }
    } else if(d->data.indexOf(file) != 0) {
        beginMoveRows(QModelIndex(), d->data.indexOf(file), d->data.indexOf(file), QModelIndex(), 0);
        d->data.move(d->data.indexOf(file), 0);
        endMoveRows();
    }
}
