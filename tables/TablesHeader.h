#ifndef CALLIGRAMOBILE_TABLESHEADER_H
#define CALLIGRAMOBILE_TABLESHEADER_H

#include <QtCore/QPointF>
#include <QtDeclarative/QDeclarativeItem>
#include <tables/Sheet.h>

class TablesCanvas;
class TablesHeader : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY(bool vertical READ vertical WRITE setVertical NOTIFY verticalChanged)
    Q_PROPERTY(QObject* canvas READ canvas WRITE setCanvas NOTIFY canvasChanged)

    public:
        explicit TablesHeader(QDeclarativeItem *parent = 0);
        void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);

        bool vertical() const;
        QObject* canvas() const;

    signals:
        void verticalChanged();
        void canvasChanged();

    public slots:
        void setVertical(bool vertical);
        void setCanvas(QObject* canvas);

    private slots:
        void sheetChanged(int newSheet);
        void docMoved();

    private:
        bool m_vertical;
        TablesCanvas* m_canvas;
        Calligra::Tables::Sheet* m_sheet;
        QPointF m_offset;
};

#endif // CALLIGRAMOBILE_TABLESHEADER_H
