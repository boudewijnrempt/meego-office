#ifndef CMCANVASINPUTPROXY_H
#define CMCANVASINPUTPROXY_H

#include <QtCore/QObject>

class QTouchEvent;
class QGestureEvent;
class QGraphicsSceneMouseEvent;
class CMCanvasControllerDeclarative;
class CMCanvasInputProxy : public QObject
{
    Q_OBJECT
public:
    CMCanvasInputProxy(CMCanvasControllerDeclarative* controller, QObject* parent = 0);
    virtual ~CMCanvasInputProxy();

    void handleMouseMoveEvent(QGraphicsSceneMouseEvent* event);
    void handleGesture(QGestureEvent* event);
    void handleTouchBegin(QTouchEvent *event);
    void handleTouchUpdate(QTouchEvent *event);
    void handleTouchEnd(QTouchEvent *event);

    CMCanvasControllerDeclarative* canvasController() const;
    void setCanvasController(CMCanvasControllerDeclarative* newController);

    bool updateCanvas() const;
    void setUpdateCanvas(bool newUpdate);

Q_SIGNALS:
    void nextPage();
    void previousPage();

private:
    class Private;
    Private * const d;
};

#endif // CMCANVASINPUTPROXY_H
