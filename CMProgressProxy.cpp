#include "CMProgressProxy.h"

CMProgressProxy::CMProgressProxy(QObject *parent)
    : QObject(parent)
{

}

CMProgressProxy::~CMProgressProxy()
{

}
 
void CMProgressProxy::setFormat(const QString& format)
{
    Q_UNUSED(format)
}

void CMProgressProxy::setRange(int minimum, int maximum)
{
    Q_UNUSED(minimum)
    Q_UNUSED(maximum)
}

void CMProgressProxy::setValue(int value)
{
    emit valueChanged(value);
}

int CMProgressProxy::maximum() const
{
    return -1;
}

