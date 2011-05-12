#ifndef CM_PROCESS_INPUT_INTERFACE_H
#define CM_PROCESS_INPUT_INTERFACE_H

#include "CMCanvasInputProxy.h"
#include <QtCore/QObject>

class QPointF;
class CMProcessInputInterface
{
public:
    CMProcessInputInterface() { }
    virtual ~CMProcessInputInterface() { }
    
    virtual void onSingleTap(const QPointF &location) = 0;
    virtual void onLongTap(const QPointF &location) = 0;
    virtual void onDoubleTap(const QPointF &location) = 0;
    virtual void onPanBegin() { }
    virtual void onPanEnd() { }

    template <typename T>
    static void setupConnections( CMCanvasInputProxy* sender, T* receiver) {
        QObject::connect(sender, SIGNAL(singleTapGesture(QPointF)), receiver, SLOT(onSingleTap(QPointF)));
        QObject::connect(sender, SIGNAL(longTapGesture(QPointF)), receiver, SLOT(onLongTap(QPointF)));
        QObject::connect(sender, SIGNAL(doubleTapGesture(QPointF)), receiver, SLOT(onDoubleTap(QPointF)));
        QObject::connect(sender, SIGNAL(beginPanGesture()), receiver, SLOT(onBeginPan()));
        QObject::connect(sender, SIGNAL(endPanGesture()), receiver, SLOT(onEndPan()));
    }
};

#endif //CM_PROCESS_INPUT_INTERFACE_H