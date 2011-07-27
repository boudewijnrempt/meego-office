#ifndef CANVASCONTROLLERDECLARATIVE_H
#define CANVASCONTROLLERDECLARATIVE_H

#include <QtDeclarative/QDeclarativeItem>
#include <KoCanvasController.h>

class CanvasInputProxy;
class KoZoomHandler;
class KoZoomController;
class KoViewConverter;

class CanvasControllerDeclarative : public QDeclarativeItem, public KoCanvasController
{
    Q_OBJECT
    Q_PROPERTY(QString file READ file WRITE setFile)
    Q_PROPERTY(int visibleWidth READ visibleWidth)
    Q_PROPERTY(int visibleHeight READ visibleHeight)

    Q_PROPERTY(QSize documentSize READ documentSize NOTIFY documentSizeChanged)
    Q_PROPERTY(QPoint documentOffset READ getDocumentOffset)

    Q_PROPERTY(QDeclarativeItem* verticalScrollHandle READ verticalScrollHandle WRITE setVerticalScrollHandle)
    Q_PROPERTY(QDeclarativeItem* horizontalScrollHandle READ horizontalScrollHandle WRITE setHorizontalScrollHandle)

    Q_PROPERTY(QDeclarativeItem* selectionCursorHandle READ selectionCursorHandle WRITE setSelectionCursorHandle)
    Q_PROPERTY(QDeclarativeItem* selectionAnchorHandle READ selectionAnchorHandle WRITE setSelectionAnchorHandle)
    
    Q_PROPERTY(bool canHavePageNotes READ canHavePageNotes NOTIFY canHavePageNotesChanged)
    Q_PROPERTY(QString currentPageNotes READ currentPageNotes NOTIFY currentPageNotesChanged)

    Q_PROPERTY(int zoomLevel READ zoomLevel WRITE setZoomLevel NOTIFY zoomLevelChanged)

public:
    CanvasControllerDeclarative(QDeclarativeItem* parent = 0);
    virtual ~CanvasControllerDeclarative();

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

    QPoint getDocumentOffset();

    Q_SLOT void setForce(const QVector2D& newForce);
    QVector2D force() const;

    QDeclarativeItem *verticalScrollHandle();
    QDeclarativeItem *horizontalScrollHandle();

    QDeclarativeItem *selectionCursorHandle();
    QDeclarativeItem *selectionAnchorHandle();
    
    /**
     * This is the current zoom level in percent, as used by the zoom controller
     * The value will be between 50 and 200
     */
    int zoomLevel() const;
    
    /**
     * The height in pixels of the area obscured by the toolbar.
     */
    int visibleToolbarHeight() const;

public Q_SLOTS:
    virtual void setVerticalScrollHandle(QDeclarativeItem *handle);
    virtual void setHorizontalScrollHandle(QDeclarativeItem *handle);

    virtual void setSelectionCursorHandle(QDeclarativeItem *handle);
    virtual void setSelectionAnchorHandle(QDeclarativeItem *handle);

    /**
     * This will cap the passed zoomPercentage to 50 to 200, and is used
     * by the zoom control dialogue to set to a specific value
     * @param zoomPercentage The new value to zoom to - should be between 50 and 200 inclusive
     */
    virtual void setZoomLevel(int zoomPercentage);
    virtual void zoomOut(const QPoint& center = QPoint());
    virtual void zoomIn(const QPoint& center = QPoint());
    virtual void zoomBy(const QPoint& center, qreal zoom);
    virtual void resetZoom();
    
    /**
     * Reimplement this when you wish to provide notes for the current page
     * This is particularly useful for slide notes in Stage, but might also be
     * useful elsewhere (plus it reduces the need for checking inside QML)
     */
    virtual QString currentPageNotes() const { return QLatin1String(""); }
    /**
     * Whether or not the current document supports notes. This is normally false,
     * and is only truthified by a particular type of document
     */
    virtual bool canHavePageNotes() const { return false; }

    void setVisibleToolbarHeight(int newHeight);

Q_SIGNALS:
    void docMoved();

    void nextPage();
    void previousPage();

    void progress(int progress);
    void completed();

    void linkActivated(const QString &url);
    void textCopiedToClipboard();

    void documentSizeChanged();

    void showVerticalScrollHandle();
    void hideVerticalScrollHandle();
    void showHorizontalScrollHandle();
    void hideHorizontalScrollHandle();
    
    void currentPageNotesChanged();
    void canHavePageNotesChanged();
    
    void zoomLevelChanged();

protected:
    virtual bool eventFilter(QObject* target, QEvent* event );
    KoZoomController* zoomController(KoViewConverter* viewConverter = 0, bool recreate = false);

    CanvasInputProxy *inputProxy();

private:
    class Private;
    Private * const d;

    Q_PRIVATE_SLOT(d, void updateCanvasSize());
    Q_PRIVATE_SLOT(d, void documentOffsetMoved(const QPoint& point));
    Q_PRIVATE_SLOT(d, void timerUpdate());
};

#endif //CANVASCONTROLLERDECLARATIVE_H
