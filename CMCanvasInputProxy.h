#ifndef CMCANVASINPUTPROXY_H
#define CMCANVASINPUTPROXY_H

#include <QtCore/QObject>

class QTouchEvent;
class QGestureEvent;
class QGraphicsSceneMouseEvent;
class QPointF;
class CMCanvasControllerDeclarative;
class CMCanvasInputProxy : public QObject
{
    Q_OBJECT
public:

    enum GestureType {
        UnknownGesture,
        SingleTapGesture,
        DoubleTapGesture,
        LongTapGesture,
        PanGesture,
        PinchGesture,
    };

    CMCanvasInputProxy(CMCanvasControllerDeclarative* controller, QObject* parent = 0);
    virtual ~CMCanvasInputProxy();

    bool handleEvent(QEvent* event);
    
    void handleMouseMoveEvent(QGraphicsSceneMouseEvent* event);
    void handleGesture(QGestureEvent* event);
    void handleTouchBegin(QTouchEvent *event);
    void handleTouchUpdate(QTouchEvent *event);
    void handleTouchEnd(QTouchEvent *event);

    CMCanvasControllerDeclarative* canvasController() const;
    void setCanvasController(CMCanvasControllerDeclarative* newController);

    bool updateCanvas() const;
    void setUpdateCanvas(bool newUpdate);

    GestureType currentGesture();

Q_SIGNALS:
    void nextPage();
    void previousPage();

    void singleTapGesture(const QPointF &location);
    void longTapGesture(const QPointF &location);
    void doubleTapGesture(const QPointF &location);
    void beginPanGesture();
    void endPanGesture();

private:
    class Private;
    Private * const d;

    Q_PRIVATE_SLOT(d, void onLongTapTimerElapsed());
};

#endif // CMCANVASINPUTPROXY_H
