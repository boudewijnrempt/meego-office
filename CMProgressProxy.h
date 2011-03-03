#ifndef CMPROGRESSPROXY_H
#define CMPROGRESSPROXY_H

#include <QtCore/QObject>
#include <KoProgressProxy.h>

class CMProgressProxy : public QObject, public KoProgressProxy
{
    Q_OBJECT
public:
    CMProgressProxy(QObject* parent = 0);
    virtual ~CMProgressProxy();
    
    virtual void setFormat(const QString& format);
    virtual void setRange(int minimum, int maximum);
    virtual void setValue(int value);
    virtual int maximum() const;

Q_SIGNALS:
    void valueChanged(int);
};

#endif // CMPROGRESSPROXY_H
