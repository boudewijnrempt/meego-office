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
    if(m_vertical)
    {
        paintVertical(p);
    }
    else
    {
        paintHorizontal(p);
    }
}

void TablesHeader::paintHorizontal(QPainter* p)
{
    const KoZoomHandler *vc = m_canvas->zoomHandler();
    const QRectF paintRect = vc->viewToDocument(QRect(0, 0, width(), height()));

    qreal xPos = 0;
    int x = 0;
    if (m_sheet->layoutDirection() == Qt::RightToLeft) {
        //Get the left column and the current x-position
        x = m_sheet->leftColumn(int(vc->unzoomItX(width()) - paintRect.x() + m_offset.x()), xPos);
        //Align to the offset
        xPos = vc->unzoomItX(width()) - xPos + vc->unzoomItX(m_offset.x());
    } else {
        //Get the left column and the current x-position
        x = m_sheet->leftColumn(int(vc->unzoomItX(paintRect.x() + m_offset.x())), xPos);
        //Align to the offset
        xPos -= vc->unzoomItX(m_offset.x());
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
    const QFontMetricsF fm(p->font());
    while (xPos <= paintRect.right() && x <= KS_colMax) {
        const Calligra::Tables::ColumnFormat *col = m_sheet->columnFormat(x);
        if (col->isHiddenOrFiltered()) {
            ++x;
            continue;
        }
        const qreal width = vc->documentToViewX(col->width());
        const QPoint pos1 = QPoint(vc->documentToViewX(xPos), 0);
        const QPoint pos2 = QPoint(vc->documentToViewX(xPos), height);
        const QRectF rect(vc->documentToViewX(xPos), 0, width, height);

        p->drawLine(pos1, pos2);

        const QString colText = m_sheet->getShowColumnNumber() ? QString::number(x) : Calligra::Tables::Cell::columnName(x);
        if (width >= fm.width(colText)) {
            p->drawText(rect, Qt::AlignCenter, colText);
        }

        xPos += col->width();

        x += deltaX;
    }
}

void TablesHeader::paintVertical(QPainter* p)
{
    const KoZoomHandler *vc = m_canvas->zoomHandler();
    const QRectF paintRect = vc->viewToDocument(QRect(0, 0, width(), height()));

    qreal yPos;
    // Get the top row and the current y-position
    int y = m_sheet->topRow(qMax<qreal>(0, int(vc->unzoomItY(paintRect.y() + m_offset.y()))), yPos);
    // Align to the offset
    yPos -= vc->unzoomItY(m_offset.y());

    const qreal width = this->width();

    // Loop through the rows, until we are out of range
    while (yPos <= paintRect.bottom() && y <= KS_rowMax) {
        const qreal rawHeight = m_sheet->rowFormats()->rowHeight(y);
        const qreal height = vc->documentToViewY(rawHeight);

        const QPoint pos1 = QPoint(0, vc->documentToViewY(yPos));
        const QPoint pos2 = QPoint(width, vc->documentToViewY(yPos));
        const QRectF rect(0, vc->documentToViewY(yPos), width, height);

        p->drawLine(pos1, pos2);
        p->save();
        QFont font = p->font();
        QFontMetricsF fm(font);
        if (height < fm.ascent() - fm.descent()) {
            // try to scale down the font to make it fit
            qreal maxSize = font.pointSizeF();
            qreal minSize = maxSize / 2;
            while (minSize > 1) {
                font.setPointSizeF(minSize);
                const QFontMetricsF fm2(font);
                if (height >= fm2.ascent() - fm2.descent())
                    break;
                minSize /= 2;
            }
            while (minSize < 0.99 * maxSize) {
                qreal middle = (maxSize + minSize) / 2;
                font.setPointSizeF(middle);
                const QFontMetricsF fm2(font);
                if (height >= fm2.ascent() - fm2.descent()) {
                    minSize = middle;
                } else {
                    maxSize = middle;
                }
            }
        }
        font.setPointSizeF(font.pointSizeF() * 0.9);
        p->setFont(font);
        fm = QFontMetricsF(font);

        if (height >= fm.ascent() - fm.descent()) {
            const QString rowText = QString::number(y);
            p->drawText(rect, Qt::AlignCenter, rowText);
        }
        p->restore();

        yPos += rawHeight;
        y++;
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
