#ifndef CALLIGRAMOBILE_STAGECANVAS_H
#define CALLIGRAMOBILE_STAGECANVAS_H

#include "../CMCanvasControllerDeclarative.h"

class CMStageCanvas : public CMCanvasControllerDeclarative
{
    Q_OBJECT
    Q_PROPERTY(QObject* document READ doc)
public:
    CMStageCanvas(QDeclarativeItem* parent = 0);
    virtual ~CMStageCanvas();

    QObject* doc() const;

public Q_SLOTS:
    void loadDocument();

private:
    class Private;
    Private * const d;

    Q_PRIVATE_SLOT(d, void setDocumentSize(const QSize& size));
};

#endif // CALLIGRAMOBILE_STAGECANVAS_H
