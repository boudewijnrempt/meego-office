#ifndef CALLIGRAMOBILE_WORDSCANVAS_H
#define CALLIGRAMOBILE_WORDSCANVAS_H

#include "../CMCanvasControllerDeclarative.h"

class CMWordsCanvas : public CMCanvasControllerDeclarative
{
    Q_OBJECT
    Q_PROPERTY(QObject* document READ doc)
    Q_PROPERTY(QString file READ file WRITE setFile)
public:
    CMWordsCanvas(QDeclarativeItem* parent = 0);
    virtual ~CMWordsCanvas();

    QObject* doc() const;
    QString file() const;
    
    void setFile(const QString &file);

private:
    class Private;
    Private * const d;
};

#endif // CALLIGRAMOBILE_WORDSCANVAS_H
