#ifndef CALLIGRAMOBILE_DOCUMENTLISTMODEL_H
#define CALLIGRAMOBILE_DOCUMENTLISTMODEL_H

#include <QAbstractListModel>
#include <QDeclarativeParserStatus>
#include <QRunnable>

class SearchThread;

class CMDocumentListModel : public QAbstractListModel, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_ENUMS(GroupBy)
    Q_INTERFACES(QDeclarativeParserStatus)

public:
    CMDocumentListModel(QObject *parent = 0);
    ~CMDocumentListModel();

    enum CustomRoles {
        FileNameRole = Qt::UserRole + 1,
        FilePathRole,
        DocTypeRole,
        SectionCategoryRole
    };

    enum GroupBy { GroupByName, GroupByDocType };

    struct DocumentInfo {
        bool operator==(const DocumentInfo &other) const { return filePath == other.filePath; }
        QString filePath;
        QString fileName;
        QString docType;
    };

    // reimp from QAbstractListModel
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    // reimp from QDeclarativeParserStatus
    void classBegin();
    void componentComplete();

public slots:
    void startSearch();
    void addDocument(const CMDocumentListModel::DocumentInfo &info);
    void addRecent(int index);

public:
    Q_INVOKABLE void groupBy(GroupBy role);

private:
    void relayout();

    QHash<QString, QString> m_docTypes;
    QList<DocumentInfo> m_recentDocuments;
    QList<DocumentInfo> m_documentInfos;
    SearchThread *m_searchThread;
    GroupBy m_groupBy;
    friend class SearchThread;
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

