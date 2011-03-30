#ifndef CMDOCUMENTTHUMBNAILLISTMODEL_H
#define CMDOCUMENTTHUMBNAILLISTMODEL_H

#include <QtCore/QAbstractItemModel>
#include <KoDocument.h>

class CMDocumentThumbnailListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QObject* document READ document WRITE setDocument);
    public:
        enum ThumbInfoRoles {
            PageThumbnailRole = Qt::UserRole + 1,
            PageNumberRole,
            PageNameRole
        };
        
        explicit CMDocumentThumbnailListModel(QObject* parent = 0);
        virtual ~CMDocumentThumbnailListModel();
        
        virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
        virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
        virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
        
        QObject* document() const;
        
        Q_INVOKABLE bool hasOwnPageNumbering() const;
    public Q_SLOTS:
        void setDocument(QObject* doc);
        
    private:
        class Private;
        Private* d;
};

#endif // CMDOCUMENTTHUMBNAILLISTMODEL_H
