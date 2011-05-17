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
        UnknownGesture = 0x1,
        SingleTapGesture = 0x2,
        DoubleTapGesture = 0x4,
        LongTapGesture = 0x8,
        PanGesture = 0x10,
        PinchGesture = 0x20,
    };
    Q_DECLARE_FLAGS(Gestures, GestureType);

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

    Gestures currentGesture();

Q_SIGNALS:
    void nextPage();
    void previousPage();

    void singleTapGesture(const QPointF &location);
    void longTapGesture(const QPointF &location);
    void longTapEnded(const QPointF &location);
    void doubleTapGesture(const QPointF &location);
    void beginPanGesture();
    void updatePanGesture(const QPointF &location);
    void endPanGesture();

private:
    class Private;
    Private * const d;

    Q_PRIVATE_SLOT(d, void onLongTapTimerElapsed());
};

Q_DECLARE_OPERATORS_FOR_FLAGS(CMCanvasInputProxy::Gestures);

#endif // CMCANVASINPUTPROXY_H
