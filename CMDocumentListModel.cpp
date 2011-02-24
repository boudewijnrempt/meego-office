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
    // ## FIXME: Among other things
    // 1. Entries must be sorted depending on the current sort
    // 2. Fetch metadata from the documents
    // 3. nameFilters must query Calligra for supported document types
    QString documentsDir = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);
    QStringList nameFilters;
    nameFilters << "*.odt" << "*.doc" << "*.odp" << "*.ppt" << "*.xls";
    QDirIterator it(documentsDir, nameFilters, QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        it.next();
        CMDocumentListModel::DocumentInfo info;
        info.fileName = it.fileName();
        info.filePath = it.filePath();
        emit documentFound(info);
    }
}

CMDocumentListModel::CMDocumentListModel(QObject *parent)
    : QAbstractListModel(parent), m_searchThread(0)
{
    qRegisterMetaType<DocumentInfo>();

    QHash<int, QByteArray> roleNames = QAbstractListModel::roleNames();
    roleNames[FileNameRole] = "fileName";
    roleNames[FilePathRole] = "filePath";
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

void CMDocumentListModel::addDocument(const DocumentInfo &info)
{
    beginInsertRows(QModelIndex(), m_documentInfos.count(), m_documentInfos.count());
    m_documentInfos.append(info);
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

