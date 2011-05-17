#ifndef CMDOCUMENTTHUMBNAILLISTMODEL_H
#define CMDOCUMENTTHUMBNAILLISTMODEL_H

#include <QtCore/QAbstractItemModel>
#include <KoDocument.h>

class CMDocumentThumbnailListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_ENUMS(DocType)
    Q_PROPERTY(QObject* document READ document WRITE setDocument);
    Q_PROPERTY(DocType docType READ docType NOTIFY docTypeChanged)
    public:
        enum DocType {
            UnknownDocType,
            WordsDocType,
            TablesDocType,
            StageDocType
        };
        
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
        DocType docType() const;
        Q_SIGNAL void docTypeChanged();
        
        Q_INVOKABLE bool hasOwnPageNumbering() const;
    public Q_SLOTS:
        void setDocument(QObject* doc);
        void setDocument(QObject* doc, QString uuid);
        void setCanvasController(QObject* newCanvasController);
        
    private:
        class Private;
        Private* d;
};

#endif // CMDOCUMENTTHUMBNAILLISTMODEL_H
