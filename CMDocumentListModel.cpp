#include "CMDocumentListModel.h"

#include <QDebug>
#include <QDesktopServices>
#include <QDirIterator>
#include <QRunnable>
#include <QThreadPool>
#include <QTimer>

#include <KDE/KGlobal>
#include <KDE/KSharedConfig>
#include <KDE/KConfigGroup>

#include <QtSparql/QSparqlConnection>
#include <QtSparql/QSparqlResult>

SearchThread::SearchThread(const QHash<QString, QString> &docTypes, QObject *parent) 
    : QObject(parent), m_abort(false), m_docTypes(docTypes)
{
}

SearchThread::~SearchThread()
{
}

void SearchThread::run()
{
    // Get documents from the device storage's document directory...
    QString documentsDir = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);
    QStringList nameFilters;
    for (QHash<QString, QString>::const_iterator it = m_docTypes.constBegin(); it != m_docTypes.constEnd(); ++it)
        nameFilters.append("*." + it.key());

    QDirIterator it(documentsDir, nameFilters, QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext() && !m_abort) {
        it.next();
        CMDocumentListModel::DocumentInfo info;
        info.fileName = it.fileName();
        info.filePath = it.filePath();
        info.docType = m_docTypes.value(info.fileName.right(3));
        emit documentFound(info);
    }

    // Get documents from the device's tracker instance
    QSparqlConnection connection("QTRACKER");
    QSparqlQuery query("");
    QSparqlResult* result = connection.exec(query);
    while (result->next()) {
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
    setRoleNames(roleNames);

    // ## FIXME : Get this from the parts
    QString docTypesText[] = { tr("Text Document"), tr("Presentation"), tr("Spreadsheet") };
    m_docTypes["odt"] = docTypesText[0];
    m_docTypes["doc"] = docTypesText[0];
    m_docTypes["odp"] = docTypesText[1];
    m_docTypes["ppt"] = docTypesText[1];
    m_docTypes["ods"] = docTypesText[2];
    m_docTypes["xls"] = docTypesText[2];
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

static bool fileNameLessThan(const CMDocumentListModel::DocumentInfo &info1, const CMDocumentListModel::DocumentInfo &info2)
{
    return info1.fileName.toLower() < info2.fileName.toLower();
}

static bool docTypeLessThan(const CMDocumentListModel::DocumentInfo &info1, const CMDocumentListModel::DocumentInfo &info2)
{
    if (info1.docType == info2.docType)
        return info1.fileName.toLower() < info2.fileName.toLower();
    return info1.docType < info2.docType;
}

void CMDocumentListModel::addDocument(const DocumentInfo &info)
{
    m_allDocumentInfos.append(info);
    
    if(m_filteredTypes.isEmpty() || info.docType == m_filteredTypes) {
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
    case SectionCategoryRole: 
        return m_groupBy == GroupByName ? info.fileName[0].toUpper() : info.docType;
    default: return QVariant();
    }
}

QVariant CMDocumentListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Vertical || role != Qt::DisplayRole)
        return QVariant();
    switch (section) {
    case 0: return tr("Filename");
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
        if(m_filteredTypes.isEmpty() || docInfo.docType == m_filteredTypes) {
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

CMDocumentListModel::Filter CMDocumentListModel::filter()
{
    return m_filter;
}

void CMDocumentListModel::setFilter(CMDocumentListModel::Filter newFilter)
{
    m_filter = newFilter;
    switch(m_filter)
    {
        case Presentations:
            m_filteredTypes = tr("Presentation");
            break;
        case Spreadsheets:
            m_filteredTypes = tr("Spreadsheet");
            break;
        case TextDocuments:
            m_filteredTypes = tr("Text Document");
            break;
        default:
            m_filteredTypes = QString();
    }
    relayout();
}

void CMDocumentListModel::componentComplete()
{
    startSearch();
}

