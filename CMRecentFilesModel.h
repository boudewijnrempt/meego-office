#ifndef CALLIGRAMOBILE_RECENTFILESMODEL_H
#define CALLIGRAMOBILE_RECENTFILESMODEL_H

#include <QtGui/QStringListModel>
#include <QtDeclarative/QDeclarativeParserStatus>
#include <QtDeclarative/QtDeclarative>
#include "calligramobile_export.h"

class CALLIGRAMOBILE_EXPORT CMRecentFilesModel : public QStringListModel, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QDeclarativeParserStatus)
    
public:
    explicit CMRecentFilesModel(QObject* parent = 0);
    virtual ~CMRecentFilesModel();

    enum Roles { FileNameRole = Qt::UserRole + 1 };

    virtual QVariant data(const QModelIndex& index, int role) const;

    virtual void classBegin();
    virtual void componentComplete();

public slots:
    void addFile(const QString& file);

private:
    class Private;
    Private * const d;
};

QML_DECLARE_TYPE(CMRecentFilesModel)

#endif // CALLIGRAMOBILE_RECENTFILESMODEL_H
