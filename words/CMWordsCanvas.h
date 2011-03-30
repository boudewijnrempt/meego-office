#ifndef CALLIGRAMOBILE_WORDSCANVAS_H
#define CALLIGRAMOBILE_WORDSCANVAS_H

#include "../CMCanvasControllerDeclarative.h"

class CMWordsCanvas : public CMCanvasControllerDeclarative
{
    Q_OBJECT
    Q_PROPERTY(QObject* document READ doc)
public:
    CMWordsCanvas(QDeclarativeItem* parent = 0);
    virtual ~CMWordsCanvas();

    QObject* doc() const;
    Q_SLOT void changePage(int newPage);

public Q_SLOTS:
    void loadDocument();

private:
    class Private;
    Private * const d;
};

#endif // CALLIGRAMOBILE_WORDSCANVAS_H
