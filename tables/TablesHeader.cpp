#include "TablesHeader.h"

#include <QtGui/QPainter>

#include <KoCanvasBase.h>
#include <KoViewConverter.h>

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
            qreal currentHeight = storage->rowHeight(i);
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
        const Calligra::Tables::ColumnFormat* col = m_sheet->firstCol();
        qreal cummulativeWidth = -vc->viewToDocumentX(m_offset.x());
        while(cummulativeWidth < visibleWidth)
        {
            QString colName( Calligra::Tables::Cell::columnName(++i) );
            QPoint pos1 = vc->documentToView(QPoint(cummulativeWidth, 0)).toPoint();
            QPoint pos2 = vc->documentToView(QPoint(cummulativeWidth, visibleHeight)).toPoint();
            QPoint pos3 = vc->documentToView(QPoint(cummulativeWidth + col->width(), visibleHeight)).toPoint();
            cummulativeWidth += col->width();
            col = col->next();
            if(cummulativeWidth >= 0)
            {
                p->drawLine(pos1, pos2);
                p->drawText(QRect(pos1, pos3), Qt::AlignCenter, colName);
            }
            if(!col)
                break;
        }
        // Draw the final line...
        QPoint pos1 = vc->documentToView(QPoint(cummulativeWidth, 0)).toPoint();
        QPoint pos2 = vc->documentToView(QPoint(cummulativeWidth, visibleHeight)).toPoint();
        p->drawLine(pos1, pos2);
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