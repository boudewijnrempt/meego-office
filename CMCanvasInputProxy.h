#ifndef CMCANVASINPUTPROXY_H
#define CMCANVASINPUTPROXY_H

class QGestureEvent;
class QGraphicsSceneMouseEvent;
class CMCanvasControllerDeclarative;
class CMCanvasInputProxy
{
public:
    CMCanvasInputProxy(CMCanvasControllerDeclarative* controller);
    virtual ~CMCanvasInputProxy();

    void handleMouseMoveEvent(QGraphicsSceneMouseEvent* event);
    void handleGesture(QGestureEvent* event);

    CMCanvasControllerDeclarative* canvasController() const;
    void setCanvasController(CMCanvasControllerDeclarative* newController);

    bool updateCanvas() const;
    void setUpdateCanvas(bool newUpdate);

private:
    class Private;
    Private * const d;
};

#endif // CMCANVASINPUTPROXY_H
