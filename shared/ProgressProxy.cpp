#include "ProgressProxy.h"

ProgressProxy::ProgressProxy(QObject *parent)
    : QObject(parent)
{

}

ProgressProxy::~ProgressProxy()
{

}
 
void ProgressProxy::setFormat(const QString& format)
{
    Q_UNUSED(format)
}

void ProgressProxy::setRange(int minimum, int maximum)
{
    Q_UNUSED(minimum)
    Q_UNUSED(maximum)
}

void ProgressProxy::setValue(int value)
{
    emit valueChanged(value);
}

int ProgressProxy::maximum() const
{
    return -1;
}

