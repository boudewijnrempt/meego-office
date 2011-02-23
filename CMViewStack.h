#ifndef CALLIGRAMOBILE_VIEWSTACK_H
#define CALLIGRAMOBILE_VIEWSTACK_H

#include <QtDeclarative/QDeclarativeItem>
#include "calligramobile_export.h"

class CALLIGRAMOBILE_EXPORT CMViewStack : public QDeclarativeItem
{
Q_OBJECT
Q_PROPERTY(QDeclarativeItem* root READ root WRITE setRoot)
Q_PROPERTY(QDeclarativeItem* current READ current)
Q_CLASSINFO("DefaultProperty", "root")

public:
    CMViewStack(QDeclarativeItem* parent = 0);
    virtual ~CMViewStack();

    QDeclarativeItem* root();
    QDeclarativeItem* current();

public Q_SLOTS:
    void pop();
    void popAll();
    void push(QDeclarativeItem* item);
    void setRoot(QDeclarativeItem* item);

private:
    class Private;
    Private * const d;

private Q_SLOTS:
    void valueChanged(qreal value);
    void animationFinished();
};

QML_DECLARE_TYPE(CMViewStack)

#endif // CALLIGRAMOBILE_VIEWSTACK_H
