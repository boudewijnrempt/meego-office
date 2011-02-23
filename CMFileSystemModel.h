#ifndef CALLIGRAMOBILE_FILESYSTEMMODEL_H
#define CALLIGRAMOBILE_FILESYSTEMMODEL_H

#include "calligramobile_export.h"

#include <QtDeclarative/QDeclarativeParserStatus>
#include <QtDeclarative/QtDeclarative>

#include <KDE/KDirSortFilterProxyModel>

class CALLIGRAMOBILE_EXPORT CMFileSystemModel : public KDirSortFilterProxyModel, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QDeclarativeParserStatus)
    Q_PROPERTY(QString rootPath READ rootPath WRITE setRootPath)
    Q_PROPERTY(QString parentFolder READ parentFolder)
    Q_PROPERTY(QString filter READ filter WRITE setFilter)
    
public:
    enum FileRoles
    {
        FileNameRole = 100,
        FilePathRole,
        FileIconRole,
        FileTypeRole,
    };
    
    explicit CMFileSystemModel(QObject* parent = 0);
    virtual ~CMFileSystemModel();

    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

    virtual void classBegin();
    virtual void componentComplete();

    virtual QString rootPath();
    virtual void setRootPath(const QString& path);

    virtual QString parentFolder();

    virtual QString filter();
    virtual void setFilter(const QString& filter);

private:
    Q_DISABLE_COPY(CMFileSystemModel)

    class Private;
    Private * const d;
};

QML_DECLARE_TYPE(CMFileSystemModel)

#endif // CALLIGRAMOBILE_FILESYSTEMMODEL_H
