#ifndef CMPROGRESSPROXY_H
#define CMPROGRESSPROXY_H

#include <QtCore/QObject>
#include <KoProgressProxy.h>

class ProgressProxy : public QObject, public KoProgressProxy
{
    Q_OBJECT
public:
    ProgressProxy(QObject* parent = 0);
    virtual ~ProgressProxy();
    
    virtual void setFormat(const QString& format);
    virtual void setRange(int minimum, int maximum);
    virtual void setValue(int value);
    virtual int maximum() const;

Q_SIGNALS:
    void valueChanged(int);
};

#endif // CMPROGRESSPROXY_H
