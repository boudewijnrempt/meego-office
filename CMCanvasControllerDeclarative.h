#ifndef CALLIGRAMOBILE_CANVASCONTROLLERDECLARATIVE_H
#define CALLIGRAMOBILE_CANVASCONTROLLERDECLARATIVE_H

#include <QtDeclarative/QDeclarativeItem>

#include <KoCanvasController.h>
#include <KoViewConverter.h>

#include "calligramobile_export.h"
#include <QVector2D>

class KoZoomHandler;
class KoZoomController;

class CALLIGRAMOBILE_EXPORT CMCanvasControllerDeclarative : public QDeclarativeItem, public KoCanvasController
{
    Q_OBJECT
    Q_PROPERTY(QString file READ file WRITE setFile)
    Q_PROPERTY(int visibleWidth READ visibleWidth)
    Q_PROPERTY(int visibleHeight READ visibleHeight)
    Q_PROPERTY(QPointF cursorPos READ cursorPos NOTIFY cursorPosChanged)
    Q_PROPERTY(QPointF anchorPos READ anchorPos NOTIFY anchorPosChanged)

public:
    CMCanvasControllerDeclarative(QDeclarativeItem* parent = 0);
    virtual ~CMCanvasControllerDeclarative();

    QString file() const;
    void setFile(const QString &f);

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
    
    KoZoomHandler* zoomHandler() const;
    void setZoomMax(qreal newZoomMax);
    qreal zoomMax() const;
    void setZoomMin(qreal newZoomMin);
    qreal zoomMin() const;

    Q_SLOT void setForce(const QVector2D& newForce);
    QVector2D force() const;

    QPointF cursorPos() const;
    QPointF anchorPos() const;

    Q_INVOKABLE void moveMarker(int which, qreal x, qreal y);

public Q_SLOTS:
    virtual void zoomOut(const QPoint& center = QPoint());
    virtual void zoomIn(const QPoint& center = QPoint());
    virtual void zoomBy(const QPoint& center, qreal zoom);
    virtual void resetZoom();

Q_SIGNALS:
    void nextPage();
    void previousPage();

    void progress(int progress);
    void completed();

    void cursorPosChanged();
    void anchorPosChanged();

    void linkActivated(const QString &url);
    void textCopiedToClipboard();
protected:
    virtual bool eventFilter(QObject* target, QEvent* event );
    KoZoomController* zoomController(KoViewConverter* viewConverter = 0, bool recreate = false);

private Q_SLOTS:
    void onHeightChanged();
    void onWidthChanged();
    void documentOffsetMoved(const QPoint& point);
    void timerUpdate();
    void onTapAndHoldGesture();

private:
    class Private;
    Private * const d;
};

#endif // CALLIGRAMOBILE_CANVASCONTROLLERDECLARATIVE_H
