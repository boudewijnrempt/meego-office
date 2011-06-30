#include "CMTablesHeader.h"
#include "CMTablesCanvas.h"

#include <tables/RowColumnFormat.h>
#include <tables/RowFormatStorage.h>
#include <tables/part/Doc.h>
#include <tables/Map.h>

#include <QPainter>

CMTablesHeader::CMTablesHeader(QDeclarativeItem *parent)
    : QDeclarativeItem(parent)
    , m_vertical(false)
    , m_sheet(0)
{
    setSize(QSizeF(40, 40));
    setFlags(flags() & ~QGraphicsItem::ItemHasNoContents);
}

void CMTablesHeader::paint(QPainter *p, const QStyleOptionGraphicsItem *, QWidget *)
{
    if(m_vertical)
    {
        if(!m_sheet)
            return;
        const Calligra::Tables::RowFormatStorage* storage = m_sheet->rowFormats();
        qreal visibleHeight = height();
        qreal visibleWidth = width();
        int i = 0;
        qreal cummulativeHeight = 0;
        while(cummulativeHeight < visibleHeight)
        {
            qreal currentHeight = storage->rowHeight(i);
            p->drawLine(0, cummulativeHeight, visibleWidth, cummulativeHeight);
            cummulativeHeight += currentHeight;
            ++i;
        }
    }
    else
    {
        QBrush lala;
        lala.setColor(Qt::green);
        lala.setStyle(Qt::VerPattern);
        p->fillRect(QRectF(QPoint(0, 0), QSizeF(width(), height())), lala);
    }
}

void CMTablesHeader::setVertical(bool vertical)
{
    if(m_vertical != vertical)
    {
        m_vertical = vertical;
        emit verticalChanged();
    }
}

bool CMTablesHeader::vertical() const
{
    return m_vertical;
}

void CMTablesHeader::setCanvas(CMTablesCanvas* canvas)
{
    qDebug() << "Setting canvas for header, with the canvas" << canvas;
    if(m_canvas)
    {
        disconnect(m_canvas, SIGNAL(docMoved()), this, SLOT(docMoved()));
        disconnect(m_canvas, SIGNAL(sheetChanged()), this, SLOT(sheetChanged()));
    }
    m_canvas = canvas;
    if(canvas)
    {
        connect(m_canvas, SIGNAL(docMoved()), this, SLOT(docMoved()));
        connect(m_canvas, SIGNAL(sheetChanged()), this, SLOT(sheetChanged()));
        sheetChanged();
    }

    m_sheet = 0;
    emit canvasChanged();
}

CMTablesCanvas* CMTablesHeader::canvas() const
{
    return m_canvas;
}

void CMTablesHeader::sheetChanged()
{
    Calligra::Tables::Doc* doc = qobject_cast< Calligra::Tables::Doc* >( m_canvas->doc() );
    if(!doc)
    {
        qDebug() << "Doc wasn't a doc! :O";
        return;
    }
    m_sheet = doc->map()->sheet( m_canvas->activeSheetIndex() );
}

void CMTablesHeader::docMoved()
{
    m_offset = m_canvas->getDocumentOffset();
}
