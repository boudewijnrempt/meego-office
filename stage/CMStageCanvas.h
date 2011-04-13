#ifndef CALLIGRAMOBILE_STAGECANVAS_H
#define CALLIGRAMOBILE_STAGECANVAS_H

#include "../CMCanvasControllerDeclarative.h"

class CMStageCanvas : public CMCanvasControllerDeclarative
{
    Q_OBJECT
    Q_PROPERTY(QObject* document READ doc)
    Q_PROPERTY(QObject* view READ view)
    Q_PROPERTY(int slide READ slide WRITE changeSlide NOTIFY slideChanged)
    Q_PROPERTY(int slideCount READ slideCount)
    
public:
    CMStageCanvas(QDeclarativeItem* parent = 0);
    virtual ~CMStageCanvas();

    QObject* doc() const;
    QObject* view() const;
    
    int slide() const;
    int slideCount() const;

public Q_SLOTS:
    void loadDocument();
    void changeSlide(int newSlide);

Q_SIGNALS:
    void slideChanged(int newSlide);

private:
    class Private;
    Private * const d;

    Q_PRIVATE_SLOT(d, void setDocumentSize(const QSize& size));
};

#endif // CALLIGRAMOBILE_STAGECANVAS_H
