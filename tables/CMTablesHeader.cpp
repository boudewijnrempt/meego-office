#include "CMTablesHeader.h"
#include "CMTablesCanvas.h"

#include <KoCanvasBase.h>
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
    setFlags(flags() & ~QGraphicsItem::ItemHasNoContents);
}

void CMTablesHeader::paint(QPainter *p, const QStyleOptionGraphicsItem *, QWidget *)
{
    if(!m_sheet)
        return;
    p->setPen(Qt::white);
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
            qreal currentHeight = storage->rowHeight(i);
            ++i;
            QString rowName = QString::number(i);
            QPoint pos1 = vc->documentToView(QPoint(0, cummulativeHeight)).toPoint();
            QPoint pos2 = vc->documentToView(QPoint(visibleWidth, cummulativeHeight)).toPoint();
            QPoint pos3 = vc->documentToView(QPoint(visibleWidth, cummulativeHeight + currentHeight)).toPoint();
            p->drawLine(pos1, pos2);
            p->drawText(QRect(pos1, pos3), Qt::AlignCenter, rowName);
            cummulativeHeight += currentHeight;
        }
    }
    else
    {
        const Calligra::Tables::ColumnFormat* col = m_sheet->firstCol();
        qreal cummulativeWidth = -vc->viewToDocumentX(m_offset.x());
        while(cummulativeWidth < visibleWidth)
        {
            QString colName( Calligra::Tables::Cell::columnName(++i) );
            QPoint pos1 = vc->documentToView(QPoint(cummulativeWidth, 0)).toPoint();
            QPoint pos2 = vc->documentToView(QPoint(cummulativeWidth, visibleHeight)).toPoint();
            QPoint pos3 = vc->documentToView(QPoint(cummulativeWidth + col->width(), visibleHeight)).toPoint();
            p->drawLine(pos1, pos2);
            p->drawText(QRect(pos1, pos3), Qt::AlignCenter, colName);
            cummulativeWidth += col->width();
            col = col->next();
            if(!col)
                break;
        }
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
    m_canvas = qobject_cast<CMTablesCanvas*>(canvas);
    if(m_canvas)
    {
        connect(m_canvas, SIGNAL(docMoved()), this, SLOT(docMoved()));
        connect(m_canvas, SIGNAL(sheetChanged(int)), this, SLOT(sheetChanged(int)));
        sheetChanged(0);
    }

    m_sheet = 0;
    emit canvasChanged();
}

CMTablesCanvas* CMTablesHeader::canvas() const
{
    return m_canvas;
}

void CMTablesHeader::sheetChanged(int newSheet)
{
    Calligra::Tables::Doc* doc = qobject_cast< Calligra::Tables::Doc* >( m_canvas->doc() );
    if(!doc)
    {
        qDebug() << "Doc wasn't a doc! :O";
        return;
    }
    m_sheet = doc->map()->sheet( newSheet );
}

void CMTablesHeader::docMoved()
{
    m_offset = m_canvas->getDocumentOffset();
}
