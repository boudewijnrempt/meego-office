#ifndef CM_PROCESS_INPUT_INTERFACE_H
#define CM_PROCESS_INPUT_INTERFACE_H

#include <QtCore/QObject>

#include "CanvasInputProxy.h"

class QPointF;
class ProcessInputInterface
{
public:
    ProcessInputInterface() { }
    virtual ~ProcessInputInterface() { }

    virtual void onSingleTap(const QPointF &location) = 0;
    virtual void onLongTap(const QPointF &location) = 0;
    virtual void onLongTapEnd(const QPointF &location) { Q_UNUSED(location); };
    virtual void onDoubleTap(const QPointF &location) = 0;
    virtual void onPanBegin() { }
    virtual void onPanEnd() { }

    template <typename T>
    static void setupConnections( CanvasInputProxy* sender, T* receiver) {
        QObject::connect(sender, SIGNAL(singleTapGesture(QPointF)), receiver, SLOT(onSingleTap(QPointF)));
        QObject::connect(sender, SIGNAL(longTapGesture(QPointF)), receiver, SLOT(onLongTap(QPointF)));
        QObject::connect(sender, SIGNAL(doubleTapGesture(QPointF)), receiver, SLOT(onDoubleTap(QPointF)));
        QObject::connect(sender, SIGNAL(longTapEnded(QPointF)), receiver, SLOT(onLongTapEnd(QPointF)));
        QObject::connect(sender, SIGNAL(beginPanGesture()), receiver, SLOT(onBeginPan()));
        QObject::connect(sender, SIGNAL(endPanGesture()), receiver, SLOT(onEndPan()));
    }
};

#endif //CM_PROCESS_INPUT_INTERFACE_H