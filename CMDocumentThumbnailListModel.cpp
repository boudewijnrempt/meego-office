#include "CMDocumentThumbnailListModel.h"

#include "CMCanvasControllerDeclarative.h"
#include "CMWordsCanvas.h"
#include "CMTablesCanvas.h"
#include "CMStageCanvas.h"
#include "CMPageThumbnailProvider.h"

#include <KoZoomHandler.h>
#include <KoCanvasBase.h>
#include <KoDocument.h>
#include <KoDocumentInfo.h>
#include <KoShapeManager.h>

#include <KoPAPageBase.h>
#include <KoPADocument.h>

#include <words/part/KWPage.h>
#include <words/part/KWDocument.h>

#include <tables/DocBase.h>
#include <tables/Map.h>
#include <tables/Sheet.h>
#include <tables/ui/SheetView.h>
#include <tables/DocBase.h>

#include <QImage>
#include <QPainter>
#include <QDeclarativeEngine>
#include <QDeclarativeContext>
#include <QDeclarativeImageProvider>

class CMDocumentThumbnailListModel::Private
{
public:
    Private()
        : docType(CMDocumentThumbnailListModel::UnknownDocType)
        , document(0)
        , engine(0)
    {}
    ~Private() {}
    
    CMDocumentThumbnailListModel::DocType docType;
    KoDocument* document;
    QDeclarativeEngine* engine;
    QWeakPointer<CMCanvasControllerDeclarative> controller;
};

CMDocumentThumbnailListModel::CMDocumentThumbnailListModel(QObject* parent)
    : QAbstractListModel(parent)
    , d(new Private())
{
    QHash<int, QByteArray> roles;
    roles[PageThumbnailRole] = "thumbnail";
    roles[PageNumberRole] = "pagenumber";
    roles[PageNameRole] = "pagename";
    setRoleNames(roles);
}

CMDocumentThumbnailListModel::~CMDocumentThumbnailListModel()
{
    delete(d);
}

QVariant CMDocumentThumbnailListModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return QVariant();

    QVariant var;
    switch(role)
    {
        case PageNameRole:
            // This is the page name, in case we've got custom page numbering
            // If we do not have custom page numbering in the document, just return an empty string
            if(qobject_cast<Calligra::Tables::DocBase*>(d->document)) {
                var = QVariant::fromValue<QString>( qobject_cast<Calligra::Tables::DocBase*>(d->document)->map()->sheet(index.row())->sheetName() );
            }
            else {
                var = QVariant::fromValue<QString>( QString() );
            }
            break;
        case PageNumberRole:
            // This is the page number
            var = QVariant::fromValue<QString>( QString::number( index.row() + 1 ) );
            break;
        case PageThumbnailRole:
            // This is the thumbnail
            // THIS IS HACKERY FOR NOW! Just doing this to have a nice unique one for the time being
            // THIS WILL NOT BE STAYING IN!
            QString someIDWhichDefinesTheDocumentUniquely = QString::number(reinterpret_cast<int64_t>(d->document));
            var = QVariant::fromValue<QString>( QString("image://pagethumbnails/%1/%2").arg(someIDWhichDefinesTheDocumentUniquely).arg(index.row() + 1) );
            break;
    }
    return var;
}

int CMDocumentThumbnailListModel::rowCount(const QModelIndex& parent) const
{
    // This is no tree, so any valid variant will have no children
    if(parent.isValid())
        return 0;

    if(d->document)
    {
        if(qobject_cast<Calligra::Tables::DocBase*>(d->document)) {
            return qobject_cast<Calligra::Tables::DocBase*>(d->document)->map()->count();
        }
        return d->document->pageCount();
    }

    // If we don't have a valid document, we'll also not have any pages
    return 0;
}

QVariant CMDocumentThumbnailListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QAbstractItemModel::headerData(section, orientation, role);
}

void CMDocumentThumbnailListModel::setCanvasController(QObject* newCanvasController)
{
    QDeclarativeContext* context = QDeclarativeEngine::contextForObject(newCanvasController);
    d->engine = context->engine();
    CMCanvasControllerDeclarative* newController = qobject_cast<CMCanvasControllerDeclarative*>(newCanvasController);
    d->controller = QWeakPointer<CMCanvasControllerDeclarative>(newController);
    if(newController)
    {
        qDebug() << "New controller, set successfully! Type" << d->controller.data()->metaObject()->className();
    }
    else
    {
        qDebug() << "New controller, pah!";
    }
}

void CMDocumentThumbnailListModel::setDocument(QObject* doc)
{
    if(qobject_cast<KoDocument*>(doc))
    {
        d->document = qobject_cast<KoDocument*>(doc);
        CMPageThumbnailProvider* provider = dynamic_cast<CMPageThumbnailProvider*>(d->engine->imageProvider(QLatin1String("pagethumbnails")));
        
        KoPADocument *stageDocument = qobject_cast<KoPADocument*>(d->document);
        Calligra::Tables::DocBase *tablesDocument = qobject_cast<Calligra::Tables::DocBase*>(d->document);
        KWDocument *wordsDocument = qobject_cast<KWDocument*>(d->document);

        //QString pageNumber = id.section('/', 1);
        if(stageDocument) {
            d->docType = CMDocumentThumbnailListModel::StageDocType;
            emit docTypeChanged();
            QSize thumbSize(240, 200);
            QString someIDWhichDefinesTheDocumentUniquely = QString::number(reinterpret_cast<int64_t>(d->document));
            int i = 0;
            foreach(KoPAPageBase *page, stageDocument->pages(false)) {
                QString id = QString("%1/%2").arg(someIDWhichDefinesTheDocumentUniquely).arg(++i);
                provider->addThumbnail(id, page->thumbnail(thumbSize).toImage());
            }
        }
        else if(tablesDocument) {
            d->docType = CMDocumentThumbnailListModel::TablesDocType;
            emit docTypeChanged();
            QSize thumbSize(200, 200);
            QString someIDWhichDefinesTheDocumentUniquely = QString::number(reinterpret_cast<int64_t>(d->document));
            if(tablesDocument->map()) {
                int i = 0;
                foreach(Calligra::Tables::Sheet* sheet, tablesDocument->map()->sheetList()) {
                    QPixmap pix(thumbSize);
                    pix.fill(Qt::white);
                    QRect rect(0, 0, pix.width(), pix.height());
                    QPainter p(&pix);

                    p.fillRect(rect, Qt::white);

                    Calligra::Tables::SheetView sheetView(sheet);

                    qreal zoom = 0.5;
                    KoZoomHandler zoomHandler;
                    zoomHandler.setZoom(zoom);
                    p.setClipRect(rect);
                    p.scale(zoom, zoom);
                    sheetView.setViewConverter(&zoomHandler);

                    QRectF area = zoomHandler.viewToDocument(rect);
                    QRect range = sheet->documentToCellCoordinates(area).adjusted(0, 0, 2, 2);
                    sheetView.setPaintCellRange(range);
                    sheetView.paintCells(p, area, QPointF(0,0));
                    QString id = QString("%1/%2").arg(someIDWhichDefinesTheDocumentUniquely).arg(++i);
                    provider->addThumbnail(id, pix.toImage());
                }
            }
        }
        else if(wordsDocument) {
            d->docType = CMDocumentThumbnailListModel::WordsDocType;
            emit docTypeChanged();
            QSize thumbSize(134, 200);
            CMWordsCanvas* canvas = qobject_cast<CMWordsCanvas*>(d->controller.data());
            KoShapeManager* shapeManager = canvas->canvas()->shapeManager();
            QList<KWPage> pages = wordsDocument->pageManager()->pages();
            QString someIDWhichDefinesTheDocumentUniquely = QString::number(reinterpret_cast<int64_t>(d->document));
            int i = 0;
            foreach(KWPage page, pages) {
                QImage thumb = page.thumbnail(thumbSize, shapeManager);
                QString id = QString("%1/%2").arg(someIDWhichDefinesTheDocumentUniquely).arg(++i);
                provider->addThumbnail(id, thumb);
            }
        }
    }
    reset();
}

QObject* CMDocumentThumbnailListModel::document() const
{
    return d->document;
}

CMDocumentThumbnailListModel::DocType CMDocumentThumbnailListModel::docType() const
{
    return d->docType;
}

bool CMDocumentThumbnailListModel::hasOwnPageNumbering() const
{
    bool result = false;
    if(d->document)
    {
        if(qobject_cast<Calligra::Tables::DocBase*>(d->document)) {
            result = true;
        }
        else {
            result = false;
        }
    }
    return result;
}


#include "CMDocumentThumbnailListModel.moc"
