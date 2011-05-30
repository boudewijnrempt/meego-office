#include "PDFCanvas.h"

#include <QtCore/QDebug>
#include <QtGui/QPainter>
#include <QtGui/QStyleOptionGraphicsItem>
#include <QtGui/QGraphicsScene>

#include <KoShapeManager.h>
#include <KoZoomHandler.h>

#include "PDFDocument.h"

class PDFCanvas::Private
{
public:
    Private(PDFCanvas *qq) : q(qq) { }
    
//     void updateSize();

    PDFCanvas *q;

    KoShapeManager *shapeManager;
    KoZoomHandler *viewConverter;
    
    PDFDocument *document;
    QPoint documentOffset;
};

PDFCanvas::PDFCanvas(PDFDocument *document, QGraphicsItem *parentItem)
    : QGraphicsWidget(parentItem), KoCanvasBase(this), d(new Private(this))
{
    d->document = document;

    d->shapeManager = new KoShapeManager(this);
    d->viewConverter = new KoZoomHandler();

    //setFlag(QGraphicsItem::ItemHasNoContents, false);
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

void PDFCanvas::addCommand ( QUndoCommand* command )
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

void PDFCanvas::update()
{
    scene()->update(mapToScene(geometry()).boundingRect());
}

void PDFCanvas::paint ( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget )
{
    painter->fillRect(geometry(), Qt::darkGray);
    
    painter->translate(-d->documentOffset);
    
    int pages = d->document->pageCount();
    qreal width = d->document->documentSize().width() * d->viewConverter->zoom();
    qreal height = (d->document->documentSize().height() / pages - 10) * d->viewConverter->zoom();

    QFont font = painter->font();
    font.setPixelSize(height/10);
    painter->setFont(font);
    int textWidth = painter->fontMetrics().width("Loading...");
    
    painter->setBrush(QBrush(Qt::white));
    painter->setPen(QPen(Qt::lightGray));
    for(int i = 0; i < pages; ++i) {
        painter->drawRect(0, i * (height + 10), width, height);
        painter->drawText((width/2) - textWidth / 2, i * (height + 10) + height/2, "Loading...");
    }

    int pageOne = int(d->documentOffset.y() / height);
    int pageTwo = int((d->documentOffset.y() + height) / height);
    
    PDFDocument::PDFPage *page = d->document->page(pageOne, true);
    if(!page) {
        return;
    }
    painter->drawImage(QRectF(0, (height + 10) * pageOne, width, height), page->image);

    page = d->document->page(pageTwo, true);
    if(!page) {
        return;
    }
    painter->drawImage(QRectF(0, (height + 10) * pageTwo, width, height), page->image);
}

#include "PDFCanvas.moc"
