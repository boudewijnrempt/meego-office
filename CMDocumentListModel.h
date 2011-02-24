#ifndef CALLIGRAMOBILE_DOCUMENTLISTMODEL_H
#define CALLIGRAMOBILE_DOCUMENTLISTMODEL_H

#include <QAbstractListModel>

class CMDocumentListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    CMDocumentListModel(QObject *parent = 0);
    ~CMDocumentListModel();

    enum CustomRoles {
        FileNameRole = Qt::UserRole + 1,
        FilePathRole
    };

    // reimp
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

private:
    struct DocumentInfo {
        QString filePath;
        QString fileName;
    };
    QList<DocumentInfo> m_documentInfos;
};

#endif // CALLIGRAMOBILE_DOCUMENTLISTMODEL_H

