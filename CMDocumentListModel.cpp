#include "CMDocumentListModel.h"

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

#include <QtSparql/QSparqlConnection>
#include <QtSparql/QSparqlResult>
#include <QtSparql/QSparqlError>
#include <kfileitem.h>


QDebug operator<<(QDebug dbg, const CMDocumentListModel::DocumentInfo& d) { 
    dbg.nospace() << d.filePath << "," << d.fileName << "," << d.docType << "," << d.fileSize << "," << d.authorName << "," << d.accessedTime << "," << d.modifiedTime << "," << d.uuid;
    return dbg.space();
};

const QString SearchThread::textDocumentType = QString("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#PaginatedTextDocument");
const QString SearchThread::presentationType = QString("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#PresentationDocument");
const QString SearchThread::spreadsheetType = QString("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#SpreadsheetDocument");

SearchThread::SearchThread(const QHash<QString, CMDocumentListModel::DocumentType> &docTypes, QObject *parent) 
    : QObject(parent), m_abort(false), m_docTypes(docTypes)
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
 		"{ ?u a nfo:PaginatedTextDocument } UNION { ?u a nfo:Presentation } UNION { ?u a nfo:Spreadsheet }"
	" }");
    QSparqlResult* result = connection.exec(query);
    result->waitForFinished();
    if(!result->hasError())
    {
        while (result->next() && !m_abort) {
            //qDebug() << result;
            CMDocumentListModel::DocumentInfo info;
            info.fileName = result->binding(0).value().toString();
            info.filePath = result->binding(1).value().toString();
            info.docType = m_docTypes.value(info.filePath.split('.').last());
            info.fileSize = result->binding(2).value().toString();
            info.authorName = "-";
            info.accessedTime = result->binding(3).value().toDateTime();
            info.modifiedTime = result->binding(4).value().toDateTime();
            info.uuid = result->binding(6).value().toString();
            /*QString type = result->binding(5).value().toString();
	    qDebug() << type;
		//.split(',').last();
	    if(type == textDocumentType) {
                info.docType = CMDocumentListModel::TextDocumentType;
            } else if(type == presentationType) {
                info.docType = CMDocumentListModel::PresentationType;
            } else if(type == spreadsheetType) {
                info.docType = CMDocumentListModel::SpreadsheetType;
            } else {
                info.docType = CMDocumentListModel::UnknownType;
            }

	    qDebug() << result->binding(6).value();
	    qDebug() << result->binding(7).value();
	    qDebug() << result->binding(8).value();*/

            emit documentFound(info);
        }
        emit finished();
        return;
    }
    else
        qDebug() << "Error while querying Tracker:" << result->lastError().message();
    
    // Query Virtuoso if available... This allows us to test on desktop so...
/*    QSparqlConnectionOptions options2;
    options2.setDatabaseName("DRIVER=/usr/lib/virtodbc_r.so");
    QSparqlConnection connection2("QVIRTUOSO", options2);
    QSparqlQuery query2(
        "SELECT nfo:fileName(?u) nie:url(?u) nfo:fileSize(?u) nco:creator(?u) nfo:fileLastAccessed(?u) nfo:fileLastModified(?u)"
        "WHERE { { ?u a nfo:PaginatedTextDocument } UNION { ?u a nfo:Presentation } UNION { ?u a nfo:Spreadsheet } }");
    QSparqlResult* result2 = connection2.exec(query2);
    result2->waitForFinished();
    if(!result2->hasError())
    {
        while (result2->next() && !m_abort) {
            CMDocumentListModel::DocumentInfo info;
            info.fileName = result2->binding(0).value().toString();
            info.filePath = result2->binding(1).value().toString();
<<<<<<< Updated upstream
            info.docType = m_docTypes.value(info.filePath.split('.').last());
=======
            //info.docType = m_docTypes.value(info.fileName.right(3));
>>>>>>> Stashed changes
            info.fileSize = result2->binding(2).value().toString();
            info.authorName = result2->binding(3).value().toString();
            if(info.authorName.isEmpty())
                info.authorName = "-";
            info.accessedTime = result2->binding(4).value().toDateTime();
            info.modifiedTime = result2->binding(5).value().toDateTime();
            emit documentFound(info);
        }

        emit finished();
        return;
    }
    else
        qDebug() << "Error while querying Virtuoso:" << result2->lastError().message();
*/
// Keeping this code around, in case Tracker later blows up horribly and we
// have to rapidly reenable the filesystem only support
    // Get documents from the device storage's document directory...
    QString documentsDir = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);
    QStringList nameFilters;
    for (QHash<QString, CMDocumentListModel::DocumentType>::const_iterator it = m_docTypes.constBegin(); it != m_docTypes.constEnd(); ++it)
        nameFilters.append("*." + it.key());

    QDirIterator it(documentsDir, nameFilters, QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext() && !m_abort) {
        it.next();
        CMDocumentListModel::DocumentInfo info;
        info.fileName = it.fileName();
        info.authorName = "-";
        info.filePath = it.filePath();
        info.modifiedTime = it.fileInfo().lastModified();
        info.accessedTime = it.fileInfo().lastRead();
        info.fileSize = QString("%1").arg(it.fileInfo().size());
        info.docType = m_docTypes.value(info.filePath.split('.').last());
        info.uuid = "not known...";
        emit documentFound(info);
    }

    emit finished();
}

CMDocumentListModel::CMDocumentListModel(QObject *parent)
    : QAbstractListModel(parent), m_groupBy(GroupByName)
{
    qRegisterMetaType<DocumentInfo>();

    QHash<int, QByteArray> roleNames = QAbstractListModel::roleNames();
    roleNames[FileNameRole] = "fileName";
    roleNames[FilePathRole] = "filePath";
    roleNames[DocTypeRole] = "docType";
    roleNames[SectionCategoryRole] = "sectionCategory";
    roleNames[FileSizeRole] = "fileSize";
    roleNames[AuthorNameRole] = "authorName";
    roleNames[AccessedTimeRole] = "accessedTime";
    roleNames[ModifiedTimeRole] = "modifiedTime";
    roleNames[UUIDRole] = "uuid";
    setRoleNames(roleNames);

    m_docTypes["odt"] = TextDocumentType;
    m_docTypes["doc"] = TextDocumentType;
    m_docTypes["docx"] = TextDocumentType;
    m_docTypes["odp"] = PresentationType;
    m_docTypes["ppt"] = PresentationType;
    m_docTypes["pptx"] = PresentationType;
    m_docTypes["ods"] = SpreadsheetType;
    m_docTypes["xls"] = SpreadsheetType;
    m_docTypes["xlsx"] = SpreadsheetType;
}

CMDocumentListModel::~CMDocumentListModel()
{
    stopSearch();
}

void CMDocumentListModel::startSearch()
{
    if (m_searchThread) {
        qDebug() << "Already searching or finished search";
        return;
    }
    m_searchThread = new SearchThread(m_docTypes);
    connect(m_searchThread, SIGNAL(documentFound(CMDocumentListModel::DocumentInfo)), this, SLOT(addDocument(CMDocumentListModel::DocumentInfo)));
    connect(m_searchThread, SIGNAL(finished()), this, SLOT(searchFinished()));
    m_searchThread->setAutoDelete(false);
    QThreadPool::globalInstance()->start(m_searchThread);
}

void CMDocumentListModel::stopSearch()
{
    if (m_searchThread)
        m_searchThread->abort();
}

void CMDocumentListModel::searchFinished()
{
    Q_ASSERT(m_searchThread);
    delete m_searchThread;
    m_searchThread = 0;
}

void CMDocumentListModel::addDocument(const DocumentInfo &info)
{
    if(m_allDocumentInfos.contains(info))
    {
        qDebug() << "Attempted to add duplicate entry" << info;
        return;
    }
    
    m_allDocumentInfos.append(info);
    
    if(m_filter == UnknownType || info.docType == m_filter) {
        beginInsertRows(QModelIndex(), m_currentDocumentInfos.count() - 1, m_currentDocumentInfos.count());
        m_currentDocumentInfos.append(info);
        endInsertRows();
    }
}

int CMDocumentListModel::rowCount(const QModelIndex &parent) const
{
    return m_currentDocumentInfos.count();
}

int CMDocumentListModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return 1;
}

QVariant CMDocumentListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    const int row = index.row();
    const DocumentInfo &info = m_currentDocumentInfos[row];

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
    case SectionCategoryRole: 
        return m_groupBy == GroupByName ? info.fileName[0].toUpper() : info.docType;
    default: return QVariant();
    }
}

QString CMDocumentListModel::prettyTime(QDateTime theTime)
{
    if( theTime.date().day() == QDateTime::currentDateTime().date().day() )
        return KGlobal::locale()->formatDateTime( theTime, KLocale::FancyShortDate );
    else if( theTime.daysTo( QDateTime::currentDateTime() ) < 7 )
        return KGlobal::locale()->formatDate( theTime.date(), KLocale::FancyShortDate );
    else
        return KGlobal::locale()->formatDate( theTime.date(), KLocale::ShortDate );
}

QVariant CMDocumentListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Vertical || role != Qt::DisplayRole)
        return QVariant();
    switch (section) {
    case 0: return tr("Filename");
    case 1: return tr("Path");
    case 2: return tr("Type");
    case 3: return tr("Size");
    case 4: return tr("Author");
    case 5: return tr("Last Accessed");
    case 6: return tr("Last Modified");
    default: return QVariant();
    }
}

void CMDocumentListModel::groupBy(GroupBy role)
{
    if (m_groupBy == role)
        return;
    m_groupBy = role;
    relayout();
}

void CMDocumentListModel::relayout()
{
    emit layoutAboutToBeChanged();

    QList<DocumentInfo> newList;
    foreach(const DocumentInfo &docInfo, m_allDocumentInfos) {
        if(m_filter == UnknownType || docInfo.docType == m_filter) {
            qDebug() << docInfo.filePath;
            newList.append(docInfo);
        }
    }
    
    m_currentDocumentInfos = newList;
    emit layoutChanged();
    reset(); // ## Required for <= Qt 4.7.2
}

void CMDocumentListModel::classBegin()
{
}

CMDocumentListModel::DocumentType CMDocumentListModel::filter()
{
    return m_filter;
}

void CMDocumentListModel::setFilter(CMDocumentListModel::DocumentType newFilter)
{
    m_filter = newFilter;
    relayout();
}

void CMDocumentListModel::componentComplete()
{
    startSearch();
}

