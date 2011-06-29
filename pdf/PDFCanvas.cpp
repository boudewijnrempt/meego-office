#include "PDFCanvas.h"

#include <QtCore/QDebug>
#include <QtGui/QPainter>
#include <QtGui/QStyleOptionGraphicsItem>
#include <QtGui/QGraphicsScene>
#include <QtNetwork/QNetworkAccessManager>

#include <KoShapeManager.h>
#include <KoZoomHandler.h>

#include "PDFDocument.h"
#include "PDFPage.h"
#include "PDFSelection.h"

class PDFCanvas::Private
{
public:
    Private(PDFCanvas *qq) : q(qq), spacing(10) { }

    PDFCanvas *q;

    KoShapeManager *shapeManager;
    KoZoomHandler *viewConverter;
    
    PDFDocument *document;
    PDFSelection *selection;
    QPoint documentOffset;

    qreal spacing;
};

PDFCanvas::PDFCanvas(PDFDocument *document, QGraphicsItem *parentItem)
    : QGraphicsWidget(parentItem), KoCanvasBase(this), d(new Private(this))
{
    d->document = document;
    connect(d->document, SIGNAL(opened()), SLOT(layout()));
    connect(d->document->networkManager(), SIGNAL(finished(QNetworkReply*)), SLOT(layout()));

    d->shapeManager = new KoShapeManager(this);
    d->viewConverter = new KoZoomHandler();

    d->selection = new PDFSelection(document, this);
    d->selection->setZValue(5);
    d->selection->setVisible(false);
}

PDFCanvas::~PDFCanvas()
{

}

void PDFCanvas::addShape ( KoShape* shape )
{

}

void PDFCanvas::removeShape ( KoShape* shape )
{

}

KoShapeManager* PDFCanvas::shapeManager() const
{
    return d->shapeManager;
}

const QWidget* PDFCanvas::canvasWidget() const
{
    return 0;
}

QWidget* PDFCanvas::canvasWidget()
{
    return 0;
}

QGraphicsWidget* PDFCanvas::canvasItem()
{
    return this;
}

const QGraphicsWidget* PDFCanvas::canvasItem() const
{
    return this;
}

void PDFCanvas::setDocumentOffset ( const QPoint& offset )
{
    d->documentOffset = offset;
}

void PDFCanvas::updateInputMethodInfo()
{

}

KoUnit PDFCanvas::unit() const
{
    return KoUnit(KoUnit::Point);
}

KoViewConverter* PDFCanvas::viewConverter() const
{
    return d->viewConverter;
}

KoToolProxy* PDFCanvas::toolProxy() const
{
    return 0;
}

void PDFCanvas::updateCanvas ( const QRectF& rc )
{

}

void PDFCanvas::addCommand ( KUndo2Command* command )
{

}

void PDFCanvas::setCursor ( const QCursor& cursor )
{

}

bool PDFCanvas::snapToGrid() const
{
    return false;
}

void PDFCanvas::gridSize ( qreal* horizontal, qreal* vertical ) const
{

}

void PDFCanvas::setSpacing ( qreal spacing )
{
    d->spacing = spacing;
    layout();
}

void PDFCanvas::update()
{
    scene()->update(mapToScene(geometry()).boundingRect());
}

PDFSelection* PDFCanvas::selection()
{
    return d->selection;
}

void PDFCanvas::paint ( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget )
{
    painter->fillRect(geometry(), Qt::darkGray);
    
    painter->translate(-d->documentOffset);

    QMatrix scaled(d->viewConverter->zoom(), 0.0, 0.0, d->viewConverter->zoom(), 0.0, 0.0);
    QRectF geom(0.0, d->documentOffset.y(), geometry().width(), geometry().height());
    QList<PDFPage*> visiblePages = d->document->visiblePages(geom, scaled);
    foreach(PDFPage *page, visiblePages) {
        QPolygonF bounds = page->boundingRect() * scaled;
        page->paint(painter, bounds.boundingRect());
    }

    d->document->updateCache();
}

void PDFCanvas::layout()
{
    QList<PDFPage*> pages = d->document->allPages();
    qreal heightAccum = 0.f;
    foreach(PDFPage* page, pages) {
        page->setPositionInDocument(heightAccum);
        heightAccum += page->height() + d->spacing;
    }
    update();
}

QMatrix PDFCanvas::scaling()
{
    return QMatrix(d->viewConverter->zoom(), 0.0, 0.0, d->viewConverter->zoom(), 0.0, 0.0);
}

#include "PDFCanvas.moc"
