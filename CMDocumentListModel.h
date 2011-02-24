#ifndef CALLIGRAMOBILE_DOCUMENTLISTMODEL_H
#define CALLIGRAMOBILE_DOCUMENTLISTMODEL_H

#include <QAbstractListModel>
#include <QRunnable>

class SearchThread;

class CMDocumentListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    CMDocumentListModel(QObject *parent = 0);
    ~CMDocumentListModel();

    enum CustomRoles {
        FileNameRole = Qt::UserRole + 1,
        FilePathRole,
        DocTypeRole,
        SectionCategoryRole
    };

    struct DocumentInfo {
        QString filePath;
        QString fileName;
        QString docType;
    };

    // reimp
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

public slots:
    void startSearch();
    void addDocument(const CMDocumentListModel::DocumentInfo &info);

public:
    Q_INVOKABLE void groupBy(int role);

private:
    void relayout();

    QList<DocumentInfo> m_documentInfos;
    SearchThread *m_searchThread;
    enum { GroupByName, GroupByDocType };
    int m_groupBy;
};

Q_DECLARE_METATYPE(CMDocumentListModel::DocumentInfo);

class SearchThread : public QObject, public QRunnable
{
    Q_OBJECT
public:
    SearchThread(QObject *parent = 0);
    ~SearchThread();
    void run();

signals:
    void documentFound(const CMDocumentListModel::DocumentInfo &);
};

#endif // CALLIGRAMOBILE_DOCUMENTLISTMODEL_H

