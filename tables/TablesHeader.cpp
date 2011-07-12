#include "TablesHeader.h"

#include <QtGui/QPainter>

#include <KoCanvasBase.h>
#include <KoViewConverter.h>
#include <KoZoomHandler.h>
#include <calligra_tables_limits.h>

#include <tables/RowColumnFormat.h>
#include <tables/RowFormatStorage.h>
#include <tables/part/Doc.h>
#include <tables/Map.h>

#include "TablesCanvas.h"

TablesHeader::TablesHeader(QDeclarativeItem *parent)
    : QDeclarativeItem(parent)
    , m_vertical(false)
    , m_backgroundColor(Qt::black)
    , m_textColor(Qt::white)
    , m_sheet(0)
{
    setFlags(flags() & ~QGraphicsItem::ItemHasNoContents);
}

void TablesHeader::paint(QPainter *p, const QStyleOptionGraphicsItem *, QWidget *)
{
    // draw full background...
    p->fillRect(0, 0, width(), height(), m_backgroundColor);

    if(!m_sheet)
        return;

    p->setPen(m_textColor);
    KoViewConverter* vc = m_canvas->canvas()->viewConverter();
    qreal visibleHeight = vc->viewToDocumentY(height());
    qreal visibleWidth = vc->viewToDocumentX(width());
    int i = 0;
    if(m_vertical)
    {
        const Calligra::Tables::RowFormatStorage* storage = m_sheet->rowFormats();
        qreal cummulativeHeight = -vc->viewToDocumentY(m_offset.y());
        while(cummulativeHeight < visibleHeight)
        {
            qreal currentHeight = storage->rowHeight(i+1);
            ++i;
            QString rowName = QString::number(i);
            QPoint pos1 = vc->documentToView(QPoint(0, cummulativeHeight)).toPoint();
            QPoint pos2 = vc->documentToView(QPoint(visibleWidth, cummulativeHeight)).toPoint();
            QPoint pos3 = vc->documentToView(QPoint(visibleWidth, cummulativeHeight + currentHeight)).toPoint();
            cummulativeHeight += currentHeight;
            if(cummulativeHeight < 0)
                continue;
            p->drawLine(pos1, pos2);
            p->drawText(QRect(pos1, pos3), Qt::AlignCenter, rowName);
        }
    }
    else
    {
        paintHorizontal(p);
    }
}

void TablesHeader::paintHorizontal(QPainter* p)
{
    const QRectF paintRect = m_canvas->zoomHandler()->viewToDocument(QRect(0, 0, width(), height()));
    const KoViewConverter *vc = m_canvas->zoomHandler();

    qreal xPos = 0;
    int x = 0;
    if (m_sheet->layoutDirection() == Qt::RightToLeft) {
        //Get the left column and the current x-position
        x = m_sheet->leftColumn(int(m_canvas->zoomHandler()->unzoomItX(width()) - paintRect.x() + m_offset.x()), xPos);
        //Align to the offset
        xPos = m_canvas->zoomHandler()->unzoomItX(width()) - xPos + m_canvas->zoomHandler()->unzoomItX(m_offset.x());
    } else {
        //Get the left column and the current x-position
        x = m_sheet->leftColumn(int(m_canvas->zoomHandler()->unzoomItX(paintRect.x() + m_offset.x())), xPos);
        //Align to the offset
        xPos -= m_canvas->zoomHandler()->unzoomItX(m_offset.x());
    }

    const qreal height = this->height();

    int deltaX = 1;
    if (m_sheet->layoutDirection() == Qt::RightToLeft) {
        if (x > KS_colMax)
            x = KS_colMax;

        xPos -= m_sheet->columnFormat(x)->width();
        deltaX = -1;
    }

    //Loop through the columns, until we are out of range
    while (xPos <= paintRect.right() && x <= KS_colMax) {
        const Calligra::Tables::ColumnFormat *col = m_sheet->columnFormat(x);
        if (col->isHiddenOrFiltered()) {
            ++x;
            continue;
        }
        const qreal width = vc->documentToViewX(col->width());
        QPoint pos1 = QPoint(vc->documentToViewX(xPos), 0);
        QPoint pos2 = QPoint(vc->documentToViewX(xPos), height);
        const QRectF rect(vc->documentToViewX(xPos), 0, width, height);

        p->drawLine(pos1, pos2);

        QString colText = m_sheet->getShowColumnNumber() ? QString::number(x) : Calligra::Tables::Cell::columnName(x);
        QFontMetricsF fm(p->font());
        if (width >= fm.width(colText)) {
            p->drawText(rect, Qt::AlignCenter, colText);
        }

        xPos += col->width();

        x += deltaX;
    }
}

void TablesHeader::setVertical(bool vertical)
{
    if(m_vertical != vertical)
    {
        m_vertical = vertical;
        emit verticalChanged();
    }
}

bool TablesHeader::vertical() const
{
    return m_vertical;
}

void TablesHeader::setCanvas(QObject* canvas)
{
    // If we've already got a canvas, disconnect from it
    if(m_canvas)
    {
        disconnect(m_canvas, SIGNAL(docMoved()), this, SLOT(docMoved()));
        disconnect(m_canvas, SIGNAL(sheetChanged()), this, SLOT(sheetChanged()));
    }

    m_canvas = qobject_cast<TablesCanvas*>(canvas);
    if(m_canvas)
    {
        connect(m_canvas, SIGNAL(docMoved()), this, SLOT(docMoved()));
        connect(m_canvas, SIGNAL(sheetChanged(int)), this, SLOT(sheetChanged(int)));
        sheetChanged(0);
    }

    m_sheet = 0;
    emit canvasChanged();
}

QObject* TablesHeader::canvas() const
{
    return m_canvas;
}

void TablesHeader::sheetChanged(int newSheet)
{
    Calligra::Tables::Doc* doc = qobject_cast< Calligra::Tables::Doc* >( m_canvas->doc() );
    if(!doc)
    {
        qDebug() << "Doc wasn't a doc! :O";
        m_sheet = 0;
        return;
    }
    m_sheet = doc->map()->sheet( newSheet );
}

void TablesHeader::docMoved()
{
    m_offset = m_canvas->getDocumentOffset();
}

QColor TablesHeader::backgroundColor() const
{
    return m_backgroundColor;
}

void TablesHeader::setBackgroundColor(QColor color)
{
    m_backgroundColor = color;
    emit backgroundColorChanged();
}

QColor TablesHeader::textColor() const
{
    return m_textColor;
}

void TablesHeader::setTextColor(QColor color)
{
    m_textColor = color;
    emit textColorChanged();
}
