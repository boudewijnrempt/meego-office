#ifndef CALLIGRAMOBILE_STAGECANVAS_H
#define CALLIGRAMOBILE_STAGECANVAS_H

#include "CMCanvasControllerDeclarative.h"
#include "CMSearchingInterface.h"

class KoFindMatch;
class CMStageCanvas : public CMCanvasControllerDeclarative, public CMSearchingInterface
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

    Q_INVOKABLE virtual int matchCount();

public Q_SLOTS:
    void loadDocument();
    void changeSlide(int newSlide);

    virtual void find ( const QString& pattern );
    virtual void findFinished();
    virtual void findNext();
    virtual void findPrevious();

Q_SIGNALS:
    void slideChanged(int newSlide);
    virtual void findMatchFound ( int match );

protected:
    void handleShortTap(QPointF pos);

private:
    class Private;
    Private * const d;

    Q_PRIVATE_SLOT(d, void setDocumentSize(const QSize& size));
    Q_PRIVATE_SLOT(d, void matchFound(KoFindMatch match));
    Q_PRIVATE_SLOT(d, void update());
};

#endif // CALLIGRAMOBILE_STAGECANVAS_H
