#include "DocumentThumbnailListModel.h"

#include <QtGui/QImage>
#include <QtGui/QPainter>
#include <QtDeclarative/QDeclarativeEngine>
#include <QtDeclarative/QDeclarativeContext>
#include <QtDeclarative/QDeclarativeImageProvider>

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

#include "CanvasControllerDeclarative.h"
#include "PageThumbnailProvider.h"
#include "words/WordsCanvas.h"
#include "tables/TablesCanvas.h"
#include "stage/StageCanvas.h"

#include "pdf/PDFCanvasController.h"
#include "pdf/PDFDocument.h"
#include "pdf/PDFPage.h"
#include <QNetworkRequest>

class DocumentThumbnailListModel::Private
{
public:
    Private() : document(0), engine(0) { }
    ~Private() {}
    
    QObject *document;
    QString uuid;
    QDeclarativeEngine* engine;
    QWeakPointer<CanvasControllerDeclarative> controller;
};

DocumentThumbnailListModel::DocumentThumbnailListModel(QObject* parent)
    : QAbstractListModel(parent)
    , d(new Private())
{
    QHash<int, QByteArray> roles;
    roles[PageThumbnailRole] = "thumbnail";
    roles[PageNumberRole] = "pageNumber";
    roles[PageNameRole] = "pageName";
    roles[PageWidthRatioRole] = "pageWidthRatio";
    setRoleNames(roles);
}

DocumentThumbnailListModel::~DocumentThumbnailListModel()
{
    delete(d);
}

QVariant DocumentThumbnailListModel::data(const QModelIndex& index, int role) const
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
            if(qobject_cast<PDFDocument*>(d->document)) {
                int height = 130;
                PDFPage* page = qobject_cast<PDFDocument*>(d->document)->page(index.row());
                int width = height * (page->width() / page->height());
                var = qobject_cast<PDFDocument*>(d->document)->buildRequest("image", QString("page=%1&width=%2&height=%3").arg(index.row()).arg(width).arg(height) ).url();
            } else {
                var = QVariant::fromValue<QString>( QString("image://pagethumbnails/%1/%2").arg(d->uuid).arg(index.row() + 1) );
            }
            break;
        case PageWidthRatioRole:
            if(qobject_cast<Calligra::Tables::DocBase*>(d->document)) {
                var = QVariant::fromValue<qreal>( 1.0 );
            } else if(qobject_cast<KoPADocument*>(d->document)) {
                KoPAPageBase* page = qobject_cast<KoPADocument*>(d->document)->pageByIndex(index.row(), false);
                QSizeF size = page->size();
                var = QVariant::fromValue<qreal>( size.width() / size.height() );
            } else if(qobject_cast<KWDocument*>(d->document)) {
                KWPage page = qobject_cast<KWDocument*>(d->document)->pageManager()->page(index.row() + 1);
                var = QVariant::fromValue<qreal>( page.width() / page.height() );
            } else if(qobject_cast<KoDocument*>(d->document)) {
                KoPageLayout pl = qobject_cast<KoDocument*>(d->document)->pageLayout(index.row() + 1);
                var = QVariant::fromValue<qreal>( pl.width / pl.height );
                qDebug() << pl.height << pl.width;
            } else if(qobject_cast<PDFDocument*>(d->document)) {
                PDFPage* page = qobject_cast<PDFDocument*>(d->document)->page( index.row() );
                var = QVariant::fromValue<qreal>( page->width() / page->height() );
            } else {
                var = QVariant::fromValue<qreal>( 1.0 );
            }
            break;
    }
    return var;
}

int DocumentThumbnailListModel::rowCount(const QModelIndex& parent) const
{
    // This is no tree, so any valid variant will have no children
    if(parent.isValid())
        return 0;

    if(d->document)
    {
        if(qobject_cast<Calligra::Tables::DocBase*>(d->document)) {
            return qobject_cast<Calligra::Tables::DocBase*>(d->document)->map()->count();
        } else if(qobject_cast<KoDocument*>(d->document)) {
            return qobject_cast<KoDocument*>(d->document)->pageCount();
        } else if(qobject_cast<PDFDocument*>(d->document)) {
            return qobject_cast<PDFDocument*>(d->document)->pageCount();
        }
    }

    // If we don't have a valid document, we'll also not have any pages
    return 0;
}

QVariant DocumentThumbnailListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QAbstractItemModel::headerData(section, orientation, role);
}

void DocumentThumbnailListModel::setCanvasController(QObject* newCanvasController)
{
    QDeclarativeContext* context = QDeclarativeEngine::contextForObject(newCanvasController);
    d->engine = context->engine();
    CanvasControllerDeclarative* newController = qobject_cast<CanvasControllerDeclarative*>(newCanvasController);
    d->controller = QWeakPointer<CanvasControllerDeclarative>(newController);
//     if(newController)
//     {
//         qDebug() << "New controller, set successfully! Type" << d->controller.data()->metaObject()->className();
//     }
//     else
//     {
//         qDebug() << "New controller, pah!";
//     }
}

void DocumentThumbnailListModel::setDocument(QObject* doc)
{
    setDocument(doc, "SET YOUR UUID");
}

void DocumentThumbnailListModel::setDocument(QObject* doc, QString uuid)
{
    d->document = doc;
    d->uuid = uuid;
    PageThumbnailProvider* provider = dynamic_cast<PageThumbnailProvider*>(d->engine->imageProvider(QLatin1String("pagethumbnails")));

    KoPADocument *stageDocument = qobject_cast<KoPADocument*>(d->document);
    Calligra::Tables::DocBase *tablesDocument = qobject_cast<Calligra::Tables::DocBase*>(d->document);
    KWDocument *wordsDocument = qobject_cast<KWDocument*>(d->document);
    PDFDocument *pdfDocument = qobject_cast<PDFDocument*>(d->document);

    if(stageDocument) {
        int height = 130;
        int i = 0;
        foreach(KoPAPageBase *page, stageDocument->pages(false)) {
            QString id = QString("%1/%2").arg(d->uuid).arg(++i);
            if(provider->hasThumbnail(id))
                continue;
            QSizeF size = page->size();
            int width = height * (size.width() / size.height());
            QSize thumbSize(width, height);
            provider->addThumbnail(id, page->thumbnail(thumbSize).toImage());
        }
    } else if(tablesDocument) {
        QSize thumbSize(130, 130);
        if(tablesDocument->map()) {
            int i = 0;
            foreach(Calligra::Tables::Sheet* sheet, tablesDocument->map()->sheetList()) {
                QString id = QString("%1/%2").arg(d->uuid).arg(++i);
                if(provider->hasThumbnail(id))
                    continue;
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
                provider->addThumbnail(id, pix.toImage());
            }
        }
    } else if(wordsDocument) {
        int height = 130;
        WordsCanvas* canvas = qobject_cast<WordsCanvas*>(d->controller.data());
        KoShapeManager* shapeManager = canvas->canvas()->shapeManager();
        QList<KWPage> pages = wordsDocument->pageManager()->pages();
        int i = 0;
        foreach(KWPage page, pages) {
            QString id = QString("%1/%2").arg(d->uuid).arg(++i);
            if(provider->hasThumbnail(id))
                continue;
            int width = height * (page.width() / page.height());
            QSize thumbSize(width, height);
            QImage thumb = page.thumbnail(thumbSize, shapeManager);
            provider->addThumbnail(id, thumb);
        }
    } else if(pdfDocument) {
        int height = 130;

        QList<PDFPage*> pages = pdfDocument->allPages();
        int i = 0;
        foreach(PDFPage *page, pages) {
            QString id = QString("%1/%2").arg(d->uuid).arg(++i);
            if(provider->hasThumbnail(id))
                continue;
            int width = height * (page->width() / page->height());
            QImage thumb = page->image(width, height);
            provider->addThumbnail(id, thumb);
        }
    } else {
        qDebug() << "Unknown Document for Thumbnails";
    }

    reset();
}

QObject* DocumentThumbnailListModel::document() const
{
    return d->document;
}

bool DocumentThumbnailListModel::hasOwnPageNumbering() const
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
