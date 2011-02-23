#include "CMViewStack.h"

#include <QtCore/QStack>
#include <QtCore/QPropertyAnimation>
#include <QtCore/QParallelAnimationGroup>
#include <QtCore/QTimeLine>

class CMViewStack::Private
{
public:
    Private(CMViewStack* qq)
        : q(qq),
          root(0),
          hiding(0),
          showing(0),
          animation(0)

    { }

    void changeVisible(QDeclarativeItem* show, QDeclarativeItem* hide);

    CMViewStack* q;

    QStack<QDeclarativeItem*> items;
    QDeclarativeItem* root;

    QDeclarativeItem* hiding;
    QDeclarativeItem* showing;

    QTimeLine* animation;
    //QParallelAnimationGroup* animation;
};

CMViewStack::CMViewStack(QDeclarativeItem* parent)
    : QDeclarativeItem(parent), d(new Private(this))
{
    d->animation = new QTimeLine(500, this);
    d->animation->setFrameRange(0, 50);
    connect(d->animation, SIGNAL(valueChanged(qreal)), this, SLOT(valueChanged(qreal)));
    connect(d->animation, SIGNAL(finished()), this, SLOT(animationFinished()));
}

CMViewStack::~CMViewStack()
{
    delete d;
}

QDeclarativeItem* CMViewStack::current()
{
    return d->items.top();
}

QDeclarativeItem* CMViewStack::root()
{
    return d->root;
}

void CMViewStack::pop()
{
    if(d->items.top() == d->root) {
        return;
    }

    QDeclarativeItem* item = d->items.pop();
    if (item) {
        d->changeVisible(d->items.top(), item);
        //connect(d->animation, SIGNAL(finished()), item, SLOT(deleteLater()));
    }
}

void CMViewStack::popAll()
{
    if(d->items.count() == 0) {
        return;
    }

    QDeclarativeItem* current = 0;
    while(d->items.top() != d->root) {
        if(d->items.top()->isVisible()) {
            current = d->items.pop();
        } else {
            d->items.pop()->deleteLater();
        }
    }
    d->root->setVisible(true);

    d->changeVisible(d->root, current);
    if(current) {
        connect(d->animation, SIGNAL(finished()), current, SLOT(deleteLater()));
    }
}

void CMViewStack::push(QDeclarativeItem* item)
{
    item->setParentItem(this);
    item->setVisible(true);

    d->changeVisible(item, d->items.top());
    d->items.push(item);
}

void CMViewStack::setRoot(QDeclarativeItem* item)
{
    if(d->items.count() > 0) {
        popAll();
        d->items.pop()->deleteLater();
    }

    d->root = item;
    item->setParentItem(this);
    d->items.push(item);
}

void CMViewStack::animationFinished()
{
    d->hiding->setVisible(false);
    d->hiding = 0;
    d->showing = 0;
}

void CMViewStack::valueChanged(qreal value)
{
    if(d->hiding)
        d->hiding->setX(-(width() * value));

    if(d->showing)
        d->showing->setX(width() * (1 - value));
}

void CMViewStack::Private::changeVisible(QDeclarativeItem* show, QDeclarativeItem* hide)
{
    hiding = hide;
    showing = show;
    showing->setVisible(true);

    animation->setCurrentTime(0);
    if(animation->state() != QTimeLine::Running ) {
        animation->start();
    }
}
