#ifndef CALLIGRAMOBILE_CANVASCONTROLLERDECLARATIVE_H
#define CALLIGRAMOBILE_CANVASCONTROLLERDECLARATIVE_H

#include <QtDeclarative/QDeclarativeItem>

#include <KoCanvasController.h>
#include <KoViewConverter.h>

#include "calligramobile_export.h"

class KoZoomHandler;
class KoZoomController;

class CALLIGRAMOBILE_EXPORT CMCanvasControllerDeclarative : public QDeclarativeItem, public KoCanvasController
{
    Q_OBJECT
    Q_PROPERTY(int visibleWidth READ visibleWidth)
    Q_PROPERTY(int visibleHeight READ visibleHeight)

public:
    CMCanvasControllerDeclarative(QDeclarativeItem* parent = 0);
    virtual ~CMCanvasControllerDeclarative();

    virtual void scrollContentsBy(int dx, int dy);
    virtual QSize viewportSize() const;

    virtual void resetDocumentOffset(const QPoint& offset = QPoint());

    virtual void setDrawShadow(bool drawShadow);

    virtual void setVastScrolling(qreal factor);
    virtual void setZoomWithWheel(bool zoom);
    virtual void updateDocumentSize(const QSize& sz, bool recalculateCenter);
    virtual void setScrollBarValue(const QPoint& value);
    virtual QPoint scrollBarValue() const;
    virtual void pan(const QPoint& distance);
    virtual QPoint preferredCenter() const;
    virtual void setPreferredCenter(const QPoint& viewPoint);
    virtual void recenterPreferred();
    virtual void zoomTo(const QRect& rect);
    virtual void ensureVisible(KoShape* shape);
    virtual void ensureVisible(const QRectF& rect, bool smooth = false);
    virtual int canvasOffsetY() const;
    virtual int canvasOffsetX() const;
    virtual int visibleWidth() const;
    virtual int visibleHeight() const;
    virtual KoCanvasBase* canvas() const;
    virtual void setCanvas(KoCanvasBase* canvas);
    virtual void setMargin(int margin);

public Q_SLOTS:
    virtual void zoomOut(const QPoint& center = QPoint());
    virtual void zoomIn(const QPoint& center = QPoint());
    virtual void zoomBy(const QPoint& center, qreal zoom);
    virtual void resetZoom();

Q_SIGNALS:
    void progress(int progress);
    void completed();
    
protected:
    virtual bool eventFilter(QObject* target, QEvent* event );
    KoZoomController* zoomController(KoViewConverter* viewConverter = 0, bool recreate = false);

private:
    class Private;
    Private * const d;

private Q_SLOTS:
    void onHeightChanged();
    void onWidthChanged();
    void documentOffsetMoved(const QPoint& point);
    void timerUpdate();
};

#endif // CALLIGRAMOBILE_CANVASCONTROLLERDECLARATIVE_H
