#ifndef CALLIGRAMOBILE_DOCUMENTLISTMODEL_H
#define CALLIGRAMOBILE_DOCUMENTLISTMODEL_H

#include <QAbstractListModel>
#include <QDeclarativeParserStatus>
#include <QRunnable>
#include <QDateTime>

class SearchThread;

class CMDocumentListModel : public QAbstractListModel, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_PROPERTY(Filter filter READ filter WRITE setFilter)
    Q_ENUMS(GroupBy)
    Q_ENUMS(Filter)
    Q_INTERFACES(QDeclarativeParserStatus)

public:
    CMDocumentListModel(QObject *parent = 0);
    ~CMDocumentListModel();

    enum CustomRoles {
        FileNameRole = Qt::UserRole + 1,
        FilePathRole,
        DocTypeRole,
        SectionCategoryRole,
        FileSizeRole,
        AuthorNameRole,
        AccessedTimeRole,
        ModifiedTimeRole,
    };

    enum GroupBy { GroupByName, GroupByDocType };

    enum Filter { None, Presentations, Spreadsheets, TextDocuments };

    struct DocumentInfo {
        bool operator==(const DocumentInfo &other) const { return filePath == other.filePath; }
        QString filePath;
        QString fileName;
        QString docType;
        QString fileSize;
        QString authorName;
        QDateTime accessedTime;
        QDateTime modifiedTime;
        QString mimeType;
    };

    // reimp from QAbstractListModel
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    // reimp from QDeclarativeParserStatus
    void classBegin();
    void componentComplete();

    Filter filter();
    
    static QString prettyTime(QDateTime theTime);

public slots:
    void startSearch();
    void stopSearch();
    void addDocument(const CMDocumentListModel::DocumentInfo &info);
    void setFilter(Filter newFilter);

public:
    Q_INVOKABLE void groupBy(GroupBy role);

private slots:
    void searchFinished();

private:
    void relayout();

    QHash<QString, QString> m_docTypes;
    QList<DocumentInfo> m_allDocumentInfos;
    QList<DocumentInfo> m_currentDocumentInfos;
    SearchThread *m_searchThread;
    GroupBy m_groupBy;
    Filter m_filter;
    QString m_filteredTypes;
    friend class SearchThread;
};

Q_DECLARE_METATYPE(CMDocumentListModel::DocumentInfo);

class SearchThread : public QObject, public QRunnable
{
    Q_OBJECT
public:
    SearchThread(const QHash<QString, QString> &docTypes, QObject *parent = 0);
    ~SearchThread();

    void run();
    
    void abort() { m_abort = true; }

signals:
    void documentFound(const CMDocumentListModel::DocumentInfo &);
    void finished();

private:
    bool m_abort;
    QHash<QString, QString> m_docTypes;
};

#endif // CALLIGRAMOBILE_DOCUMENTLISTMODEL_H

