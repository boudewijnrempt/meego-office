#ifndef CALLIGRAMOBILE_WORDSCANVAS_H
#define CALLIGRAMOBILE_WORDSCANVAS_H

#include "CMCanvasControllerDeclarative.h"
#include "CMSearchingInterface.h"

class CMWordsCanvas : public CMCanvasControllerDeclarative, public CMSearchingInterface
{
    Q_OBJECT
    Q_PROPERTY(QObject* document READ doc)
    Q_PROPERTY(int page READ page WRITE changePage NOTIFY pageChanged)
    Q_PROPERTY(int pageCount READ pageCount)

public:
    CMWordsCanvas(QDeclarativeItem* parent = 0);
    virtual ~CMWordsCanvas();

    QObject* doc() const;
    int page() const;
    int pageCount() const;

    Q_INVOKABLE int matchCount();

public Q_SLOTS:
    void loadDocument();
    void changePage(int newPage);

    void find(const QString& pattern);
    void findPrevious();
    void findNext();
    void findFinished();

Q_SIGNALS:
    void pageChanged(int newPage);
    void findMatchFound(int match);
protected:
    void handleShortTap(QPointF pos);
private:
    class Private;
    Private * const d;

    Q_PRIVATE_SLOT(d, void matchFound(KoFindMatch));
    Q_PRIVATE_SLOT(d, void update());
};

#endif // CALLIGRAMOBILE_WORDSCANVAS_H
