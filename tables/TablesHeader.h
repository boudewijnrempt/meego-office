#ifndef CALLIGRAMOBILE_TABLESHEADER_H
#define CALLIGRAMOBILE_TABLESHEADER_H

#include <QtCore/QPointF>
#include <QtGui/QColor>
#include <QtDeclarative/QDeclarativeItem>
#include <tables/Sheet.h>

class TablesCanvas;
class TablesHeader : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY(bool vertical READ vertical WRITE setVertical NOTIFY verticalChanged)
    Q_PROPERTY(QObject* canvas READ canvas WRITE setCanvas NOTIFY canvasChanged)
    /// The background color of the entire item. Defaults to black.
    Q_PROPERTY(QColor backroundColor READ backgroundColor WRITE setBackgroundColor NOTIFY backgroundColorChanged)
    /// The text color (and line color) of the header items. Defaults to white.
    Q_PROPERTY(QColor textColor READ textColor WRITE setTextColor NOTIFY textColorChanged)

    public:
        explicit TablesHeader(QDeclarativeItem *parent = 0);
        void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);

        bool vertical() const;
        QObject* canvas() const;
        QColor backgroundColor() const;
        QColor textColor() const;

    signals:
        void verticalChanged();
        void canvasChanged();
        void backgroundColorChanged();
        void textColorChanged();

    public slots:
        void setVertical(bool vertical);
        void setCanvas(QObject* canvas);
        void setBackgroundColor(QColor color);
        void setTextColor(QColor color);

    private slots:
        void sheetChanged(int newSheet);
        void docMoved();

    private:
        void paintHorizontal(QPainter* p);
        void paintVertical(QPainter* p);

        bool m_vertical;
        TablesCanvas* m_canvas;
        QColor m_backgroundColor;
        QColor m_textColor;
        Calligra::Tables::Sheet* m_sheet;
        QPointF m_offset;
};

#endif // CALLIGRAMOBILE_TABLESHEADER_H
