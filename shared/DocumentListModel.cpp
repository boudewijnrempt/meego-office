#include "DocumentListModel.h"

#include <QDebug>
#include <QDesktopServices>
#include <QDirIterator>
#include <QRunnable>
#include <QThreadPool>
#include <QTimer>

#include <KDE/KGlobal>
#include <KDE/KLocale>
#include <KDE/KSharedConfig>
#include <KDE/KConfigGroup>
#include <KDE/KFileItem>

#include <QtSparql/QSparqlConnection>
#include <QtSparql/QSparqlResult>
#include <QtSparql/QSparqlError>

const QString SearchThread::textDocumentType = QString("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#PaginatedTextDocument");
const QString SearchThread::presentationType = QString("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#PresentationDocument");
const QString SearchThread::spreadsheetType = QString("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#SpreadsheetDocument");

SearchThread::SearchThread(QObject *parent)
    : QObject(parent), m_abort(false)
{
}

SearchThread::~SearchThread()
{
}

void SearchThread::run()
{
    // Get documents from the device's tracker instance
    QSparqlConnection connection("QTRACKER");
    QSparqlQuery query(
        "PREFIX nfo: <http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#> "
        "PREFIX nie: <http://www.semanticdesktop.org/ontologies/2007/01/19/nie#> "
        "PREFIX nco: <http://www.semanticdesktop.org/ontologies/2007/03/22/nco#> "
        "SELECT ?name ?path ?size ?lastAccessed ?lastModified ?type ?uuid "
        "WHERE { "
            "?u nfo:fileName ?name . "
            "?u nie:url ?path . "
            "?u nfo:fileSize ?size . "
            "?u nfo:fileLastAccessed ?lastAccessed . "
            "?u nfo:fileLastModified ?lastModified . "
            "?u rdf:type ?type . "
            "?u nie:isStoredAs ?uuid . "
            "FILTER( ?type = nfo:PaginatedTextDocument || ?type = nfo:Presentation || ?type = nfo:Spreadsheet ) . "
        "} ORDER BY ?name");
    
    QSparqlResult* result = connection.exec(query);
    result->waitForFinished();
    if(!result->hasError())
    {
        while (result->next() && !m_abort) {
            DocumentListModel::DocumentInfo info;
            info.fileName = result->binding(0).value().toString();
            info.filePath = result->binding(1).value().toString();
            info.docType = DocumentListModel::typeForFile(info.filePath);
            info.fileSize = result->binding(2).value().toString();
            info.authorName = "-";
            info.accessedTime = result->binding(3).value().toDateTime();
            info.modifiedTime = result->binding(4).value().toDateTime();
            info.uuid = result->binding(6).value().toString();
            
            /* Uncomment this once tracker starts behaving irt rdf:Type of documents.
            QString type = result->binding(5).value().toString();
	    if(type == textDocumentType) {
                info.docType = CMDocumentListModel::TextDocumentType;
            } else if(type == presentationType) {
                info.docType = CMDocumentListModel::PresentationType;
            } else if(type == spreadsheetType) {
                info.docType = CMDocumentListModel::SpreadsheetType;
            } else {
                info.docType = CMDocumentListModel::UnknownType;
            }*/

            emit documentFound(info);
        }
        emit finished();
        return;
    }

    // Get documents from the device storage's document directory...
    QString documentsDir = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);
    QStringList nameFilters;
    for (QHash<QString, DocumentListModel::DocumentType>::const_iterator it = m_docTypes.constBegin(); it != m_docTypes.constEnd(); ++it) {
        nameFilters.append("*." + it.key());
    }

    QDirIterator it(documentsDir, nameFilters, QDir::Files, QDirIterator::Subdirectories);
    int i = 0;
    while (it.hasNext() && !m_abort) {
        it.next();
        DocumentListModel::DocumentInfo info;
        info.fileName = it.fileName();
        info.authorName = "-";
        info.filePath = it.filePath();
        info.modifiedTime = it.fileInfo().lastModified();
        info.accessedTime = it.fileInfo().lastRead();
        info.fileSize = QString("%1").arg(it.fileInfo().size());
        info.docType = DocumentListModel::typeForFile(info.filePath);
        info.uuid = QString("filesystem-document-").append(i++);
        emit documentFound(info);
    }

    emit finished();
}

class DocumentListModel::Private
{
public:
    Private( DocumentListModel *qq) : q(qq), searchThread(0), filter(DocumentListModel::UnknownType) { }
    
    void relayout();

    DocumentListModel* q;
    
    QList<DocumentInfo> allDocumentInfos;
    QList<DocumentInfo> currentDocumentInfos;
    
    SearchThread *searchThread;

    DocumentType filter;
    QString searchPattern;
};

QHash<QString, DocumentListModel::DocumentType> DocumentListModel::sm_extensions = QHash<QString, DocumentListModel::DocumentType>();

DocumentListModel::DocumentListModel(QObject *parent)
    : QAbstractListModel(parent), d(new Private(this))
{
    qRegisterMetaType<DocumentListModel::DocumentInfo>();
    
    QHash<int, QByteArray> roleNames = QAbstractListModel::roleNames();
    roleNames[FileNameRole] = "fileName";
    roleNames[FilePathRole] = "filePath";
    roleNames[DocTypeRole] = "docType";
    roleNames[FileSizeRole] = "fileSize";
    roleNames[AuthorNameRole] = "authorName";
    roleNames[AccessedTimeRole] = "accessedTime";
    roleNames[ModifiedTimeRole] = "modifiedTime";
    roleNames[UUIDRole] = "uuid";
    setRoleNames(roleNames);
}

DocumentListModel::~DocumentListModel()
{
    stopSearch();
}

void DocumentListModel::startSearch()
{
    if (d->searchThread) {
        qDebug() << "Already searching or finished search";
        return;
    }
    d->searchThread = new SearchThread();
    connect(d->searchThread, SIGNAL(documentFound( DocumentListModel::DocumentInfo)), this, SLOT(addDocument( DocumentListModel::DocumentInfo)));
    connect(d->searchThread, SIGNAL(finished()), this, SLOT(searchFinished()));
    d->searchThread->setAutoDelete(false);
    QThreadPool::globalInstance()->start(d->searchThread);
}

void DocumentListModel::stopSearch()
{
    if (d->searchThread) {
        d->searchThread->abort();
        d->searchThread->deleteLater();
    }
}

void DocumentListModel::searchFinished()
{
    Q_ASSERT(d->searchThread);
    delete d->searchThread;
    d->searchThread = 0;
}

void DocumentListModel::addDocument(const DocumentInfo &info)
{
    if(d->allDocumentInfos.contains(info))
    {
        return;
    }
    
    d->allDocumentInfos.append(info);
    d->relayout();
}

int DocumentListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return d->currentDocumentInfos.count();
}

int DocumentListModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 1;
}

QVariant DocumentListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    
    const int row = index.row();
    const DocumentInfo &info = d->currentDocumentInfos[row];

    switch (role) {
        case FileNameRole: // intentional fall through
        case Qt::DisplayRole: return info.fileName;
        case FilePathRole: return info.filePath;
        case DocTypeRole: return info.docType;
        case FileSizeRole: return info.fileSize;
        case AuthorNameRole: return info.authorName;
        case AccessedTimeRole: return prettyTime(info.accessedTime);
        case ModifiedTimeRole: return prettyTime(info.modifiedTime);
        case UUIDRole: return info.uuid;
        default: return QVariant();
    }
}

QString DocumentListModel::prettyTime( const QDateTime& theTime)
{
    return KGlobal::locale()->formatDateTime( theTime, KLocale::FancyShortDate );
    
    /*if( theTime.date().day() == QDateTime::currentDateTime().date().day() )
        
    else if( theTime.daysTo( QDateTime::currentDateTime() ) < 7 )
        return KGlobal::locale()->formatDate( theTime.date(), KLocale::FancyShortDate );
    else
        return KGlobal::locale()->formatDate( theTime.date(), KLocale::ShortDate );
        */
}

QVariant DocumentListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(section)
    Q_UNUSED(orientation)
    Q_UNUSED(role)
    return QVariant();
//     if (orientation == Qt::Vertical || role != Qt::DisplayRole)
//         
//     switch (section) {
//         case 0: return tr("Filename");
//         case 1: return tr("Path");
//         case 2: return tr("Type");
//         case 3: return tr("Size");
//         case 4: return tr("Author");
//         case 5: return tr("Last Accessed");
//         case 6: return tr("Last Modified");
//         default: return QVariant();
//     }
}

// void CMDocumentListModel::groupBy(GroupBy role)
// {
//     if (m_groupBy == role)
//         return;
//     m_groupBy = role;
//     relayout();
// }

void DocumentListModel::classBegin()
{
}

DocumentListModel::DocumentType DocumentListModel::filter()
{
    return d->filter;
}

QString DocumentListModel::search()
{
    return d->searchPattern;
}

void DocumentListModel::setFilter( DocumentListModel::DocumentType newFilter)
{
    d->filter = newFilter;
    d->relayout();
}

void DocumentListModel::setSearch ( const QString& search )
{
    d->searchPattern = search;
    d->relayout();
}

void DocumentListModel::componentComplete()
{
    startSearch();
}

DocumentListModel::DocumentType DocumentListModel::typeForFile ( const QString& file )
{
    if(sm_extensions.isEmpty()) {
        sm_extensions["odt"] = TextDocumentType;
        sm_extensions["fodt"] = TextDocumentType;
        sm_extensions["doc"] = TextDocumentType;
        sm_extensions["docx"] = TextDocumentType;
        sm_extensions["odp"] = PresentationType;
        sm_extensions["fodp"] = PresentationType;
        sm_extensions["ppt"] = PresentationType;
        sm_extensions["pptx"] = PresentationType;
        sm_extensions["ods"] = SpreadsheetType;
        sm_extensions["fods"] = SpreadsheetType;
        sm_extensions["xls"] = SpreadsheetType;
        sm_extensions["xlsx"] = SpreadsheetType;
        sm_extensions["pdf"] = PDFDocumentType;
    }

    QString ext = file.split('.').last().toLower();
    if(sm_extensions.contains(ext)) {
        return sm_extensions.value(ext);
    }

    return UnknownType;
}

void DocumentListModel::Private::relayout()
{
    emit q->layoutAboutToBeChanged();

    QList<DocumentInfo> newList;
    foreach(const DocumentInfo &docInfo, allDocumentInfos) {
        if(filter == UnknownType || docInfo.docType == filter) {
            if(searchPattern.isEmpty() || docInfo.fileName.contains(searchPattern, Qt::CaseInsensitive)) {
                newList.append(docInfo);
            }
        }
    }

    currentDocumentInfos = newList;
    emit q->layoutChanged();
    q->reset(); // ## Required for <= Qt 4.7.2
}
