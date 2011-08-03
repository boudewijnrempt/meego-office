#ifndef CALLIGRAMOBILE_DOCUMENTLISTMODEL_H
#define CALLIGRAMOBILE_DOCUMENTLISTMODEL_H

#include <QAbstractListModel>
#include <QDeclarativeParserStatus>
#include <QRunnable>
#include <QDateTime>

class SearchThread;

class DocumentListModel : public QAbstractListModel, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_ENUMS(DocumentType)
    Q_INTERFACES(QDeclarativeParserStatus)

    Q_PROPERTY(DocumentType filter READ filter WRITE setFilter)
    Q_PROPERTY(QString search READ search WRITE setSearch)

public:
    DocumentListModel(QObject *parent = 0);
    ~DocumentListModel();

    enum CustomRoles {
        FileNameRole = Qt::UserRole + 1,
        FilePathRole,
        DocTypeRole,
        FileSizeRole,
        AuthorNameRole,
        AccessedTimeRole,
        ModifiedTimeRole,
        UUIDRole,
    };

    enum DocumentType
    {
        UnknownType,
	TextDocumentType,
        PresentationType,
        SpreadsheetType,
        PDFDocumentType,
    };

    struct DocumentInfo {
        bool operator==(const DocumentInfo &other) const { return filePath == other.filePath; }
        QString filePath;
        QString fileName;
        DocumentType docType;
        QString fileSize;
        QString authorName;
        QDateTime accessedTime;
        QDateTime modifiedTime;
        QString uuid;
    };

    // reimp from QAbstractListModel
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    // reimp from QDeclarativeParserStatus
    void classBegin();
    void componentComplete();

    DocumentType filter();
    QString search();

    static QString prettyTime(const QDateTime &theTime);
    static DocumentType typeForFile(const QString &file);

public slots:
    void startSearch();
    void stopSearch();
    void addDocument(const DocumentListModel::DocumentInfo &info);
    void setFilter(DocumentType newFilter);
    void setSearch(const QString &search);
    void refresh();

private slots:
    void searchFinished();

private:
    friend class SearchThread;

    class Private;
    const QScopedPointer<Private> d;

    static QHash<QString, DocumentType> sm_extensions;
};

Q_DECLARE_METATYPE( DocumentListModel::DocumentInfo);

class SearchThread : public QObject, public QRunnable
{
    Q_OBJECT
public:
    SearchThread( QObject* parent = 0);
    ~SearchThread();

    void run();
    void abort() { m_abort = true; }

signals:
    void documentFound(const DocumentListModel::DocumentInfo &);
    void finished();

private:
    bool m_abort;
    QHash<QString, DocumentListModel::DocumentType> m_docTypes;
    static const QString textDocumentType;
    static const QString presentationType;
    static const QString spreadsheetType;
};

#endif // CALLIGRAMOBILE_DOCUMENTLISTMODEL_H

