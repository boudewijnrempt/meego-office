#include "CMDocumentListModel.h"

#include <QDebug>
#include <QDesktopServices>
#include <QDirIterator>
#include <QRunnable>
#include <QThreadPool>

SearchThread::SearchThread(QObject *parent) : QObject(parent)
{
}

SearchThread::~SearchThread()
{
}

void SearchThread::run()
{
    // ## FIXME : Get this from the parts
    static const char *docTypesText[] = {
        QT_TRANSLATE_NOOP("CMDocumentListModel", "Words Document"),
        QT_TRANSLATE_NOOP("CMDocumentListModel", "Stage Document"),
        QT_TRANSLATE_NOOP("CMDocumentListModel", "Tables Document")
    };
    QHash<QString, QString> docTypes;
    docTypes["odt"] = CMDocumentListModel::tr(docTypesText[0]);
    docTypes["doc"] = CMDocumentListModel::tr(docTypesText[0]);
    docTypes["odp"] = CMDocumentListModel::tr(docTypesText[1]);
    docTypes["ppt"] = CMDocumentListModel::tr(docTypesText[1]);
    docTypes["ods"] = CMDocumentListModel::tr(docTypesText[2]);
    docTypes["xls"] = CMDocumentListModel::tr(docTypesText[2]);

    // ## FIXME: Among other things
    // 1. Entries must be sorted depending on the current sort
    // 2. Fetch metadata from the documents
    QString documentsDir = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);
    QStringList nameFilters;
    for (QHash<QString, QString>::const_iterator it = docTypes.constBegin(); it != docTypes.constEnd(); ++it)
        nameFilters.append("*." + it.key());

    QDirIterator it(documentsDir, nameFilters, QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        it.next();
        CMDocumentListModel::DocumentInfo info;
        info.fileName = it.fileName();
        info.filePath = it.filePath();
        info.docType = docTypes.value(info.fileName.right(3));
        emit documentFound(info);
    }
}

CMDocumentListModel::CMDocumentListModel(QObject *parent)
    : QAbstractListModel(parent), m_searchThread(0), m_groupBy(GroupByName)
{
    qRegisterMetaType<DocumentInfo>();

    QHash<int, QByteArray> roleNames = QAbstractListModel::roleNames();
    roleNames[FileNameRole] = "fileName";
    roleNames[FilePathRole] = "filePath";
    roleNames[DocTypeRole] = "docType";
    roleNames[SectionCategoryRole] = "sectionCategory";
    setRoleNames(roleNames);
}

CMDocumentListModel::~CMDocumentListModel()
{
}

void CMDocumentListModel::startSearch()
{
    if (m_searchThread) {
        qDebug() << "Already searching or finished search";
        return;
    }
    m_searchThread = new SearchThread(this);
    connect(m_searchThread, SIGNAL(documentFound(CMDocumentListModel::DocumentInfo)), this, SLOT(addDocument(CMDocumentListModel::DocumentInfo)));
    m_searchThread->setAutoDelete(true);
    QThreadPool::globalInstance()->start(m_searchThread);
}

static bool fileNameLessThan(const CMDocumentListModel::DocumentInfo &info1, const CMDocumentListModel::DocumentInfo &info2)
{
    return info1.fileName.toLower() < info2.fileName.toLower();
}

static bool docTypeLessThan(const CMDocumentListModel::DocumentInfo &info1, const CMDocumentListModel::DocumentInfo &info2)
{
    return info1.docType < info2.docType;
}

void CMDocumentListModel::addDocument(const DocumentInfo &info)
{
    bool (*lessThanFunc)(const CMDocumentListModel::DocumentInfo &, const CMDocumentListModel::DocumentInfo &) = 0;
    if (m_groupBy == GroupByName)
        lessThanFunc = fileNameLessThan;
    else
        lessThanFunc = docTypeLessThan;
    QList<DocumentInfo>::iterator it = qLowerBound(m_documentInfos.begin(), m_documentInfos.end(), info, fileNameLessThan);
    const int pos = it - m_documentInfos.begin();
    beginInsertRows(QModelIndex(), pos, pos);
    m_documentInfos.insert(it, info);
    endInsertRows();
}

int CMDocumentListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_documentInfos.count();
}

int CMDocumentListModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return 1;
}

QVariant CMDocumentListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || m_documentInfos.isEmpty())
        return QVariant();
    const int row = index.row();
    const DocumentInfo &info = m_documentInfos[row];
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

void CMDocumentListModel::groupBy(int role)
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
    for (int i = 0; i < m_documentInfos.count(); i++) {
        QString section;
        if (m_groupBy == GroupByName)
            section = m_documentInfos[i].fileName[0].toUpper();
        else
            section = m_documentInfos[i].docType;
        map[section].append(i);
    }
    QList<DocumentInfo> newList;
    for (QMap<QString, QList<int> >::const_iterator it = map.constBegin(); it != map.constEnd(); ++it) {
        QList<int> indices = it.value();
        foreach(int index, indices) {
            newList.append(m_documentInfos[index]);
        }
    }
    m_documentInfos = newList;
    emit layoutChanged();
}

