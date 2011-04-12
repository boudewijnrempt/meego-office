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

SearchThread::SearchThread(const QHash<QString, QString> &docTypes, QObject *parent) 
    : QObject(parent), m_abort(false), m_docTypes(docTypes)
{
}

SearchThread::~SearchThread()
{
}

void SearchThread::run()
{
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
    QString docTypesText[] = { tr("Words Document"), tr("Stage Document"), tr("Tables Document") };
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

    KConfigGroup group(KGlobal::config(), "Recent Files");
    int index = 0;
    foreach(const DocumentInfo &info, m_recentDocuments) {
        group.writeEntry(QString().setNum(index), info.filePath);
        index++;
    }
    group.sync();
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
    bool (*lessThanFunc)(const CMDocumentListModel::DocumentInfo &, const CMDocumentListModel::DocumentInfo &) = 0;
    if (m_groupBy == GroupByName)
        lessThanFunc = fileNameLessThan;
    else
        lessThanFunc = docTypeLessThan;
    QList<DocumentInfo>::iterator it = qLowerBound(m_allDocumentInfos.begin(), m_allDocumentInfos.end(), info, fileNameLessThan);
    const int pos = it - m_allDocumentInfos.begin() + m_recentDocuments.count();
    m_allDocumentInfos.insert(it, info);
    
    if(m_filteredTypes.isEmpty() || info.docType == m_filteredTypes) {
        beginInsertRows(QModelIndex(), m_currentDocumentInfos.count() - 1, m_currentDocumentInfos.count());
        m_currentDocumentInfos.append(info);
        endInsertRows();
    }
}

int CMDocumentListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_recentDocuments.count() + m_currentDocumentInfos.count();
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
        if (row < m_recentDocuments.count())
            return tr("Recently viewed");
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
    QMap<QString, QList<int> > map;
    for (int i = 0; i < m_allDocumentInfos.count(); i++) {
        QString section;
        if (m_groupBy == GroupByName)
            section = m_allDocumentInfos[i].fileName[0].toUpper();
        else
            section = m_allDocumentInfos[i].docType;
        map[section].append(i);
    }
    QList<DocumentInfo> newList;
    for (QMap<QString, QList<int> >::const_iterator it = map.constBegin(); it != map.constEnd(); ++it) {
        QList<int> indices = it.value();
        foreach(int index, indices) {
            if(m_filteredTypes.isEmpty() || m_allDocumentInfos[index].docType == m_filteredTypes) {
                newList.append(m_allDocumentInfos[index]);
            }
        }
    }
    m_currentDocumentInfos = newList;
    emit layoutChanged();
    reset(); // ## Required for <= Qt 4.7.2
}

void CMDocumentListModel::addRecent(const QString &path)
{
//     QFileInfo fi(path);
//     CMDocumentListModel::DocumentInfo info;
//     info.fileName = fi.fileName();
//     info.filePath = fi.absoluteFilePath();
//     info.docType = m_docTypes.value(info.fileName.right(3));
//     addRecent(info);
}

void CMDocumentListModel::addRecent(int index)
{
//     Q_ASSERT(index >= 0 && index < rowCount());
//     DocumentInfo info = (index >= m_recentDocuments.count()) ? m_allDocumentInfos[index - m_recentDocuments.count()] : m_recentDocuments[index];
//     addRecent(info);
}

void CMDocumentListModel::addRecent(const DocumentInfo &info)
{
//     const int MAX_RECENT = 5;
//     int toRemove = -1;
//     for (int i = 0; i < m_recentDocuments.count(); ++i) {
//         if (m_recentDocuments[i] == info) {
//             toRemove = i;
//             break;
//         }
//     }
// 
//     if (toRemove == -1 && m_recentDocuments.count() == MAX_RECENT)
//         toRemove = MAX_RECENT - 1;
// 
//     if (toRemove != -1) {
//         beginRemoveRows(QModelIndex(), toRemove, toRemove);
//         m_recentDocuments.removeAt(toRemove);
//         endRemoveRows();
//     }
//  
//     beginInsertRows(QModelIndex(), 0, 0);
//     m_recentDocuments.prepend(info);
//     endInsertRows();
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
            m_filteredTypes = tr("Stage Document");
            break;
        case Spreadsheets:
            m_filteredTypes = tr("Tables Document");
            break;
        case TextDocuments:
            m_filteredTypes = tr("Words Document");
            break;
        default:
            m_filteredTypes = QString();
    }
    relayout();
}

void CMDocumentListModel::componentComplete()
{
//     KConfigGroup group(KGlobal::config(), "Recent Files");
// 
//     QStringList keys = group.keyList();
//     for (int i = keys.count() - 1; i >= 0; --i) {
//         DocumentInfo info;
//         info.filePath = group.readEntry(keys[i]);
//         QFileInfo fi(info.filePath);
//         if (!fi.exists())
//             continue;
//         info.fileName = fi.fileName();
//         info.docType = m_docTypes.value(info.fileName.right(3));
//         beginInsertRows(QModelIndex(), 0, 0);
//         m_recentDocuments.prepend(info);
//         endInsertRows();
//     }

    startSearch();
}

