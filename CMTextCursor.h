#ifndef CALLIGRAMOBILE_TEXTCURSOR_H
#define CALLIGRAMOBILE_TEXTCURSOR_H

#include "calligramobile_export.h"
#include <QObject>
#include <QTextCursor>

class CALLIGRAMOBILE_EXPORT CMTextCursor : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal anchorX READ anchorX NOTIFY anchorXChanged)
    Q_PROPERTY(qreal anchorY READ anchorY NOTIFY anchorYChanged)
    Q_PROPERTY(qreal positionX READ positionX NOTIFY positionXChanged)
    Q_PROPERTY(qreal positionY READ positionY NOTIFY positionYChanged)
    Q_PROPERTY(bool valid READ isValid NOTIFY validChanged)

public:
    CMTextCursor();
    ~CMTextCursor();

    qreal anchorX() const;
    qreal anchorY() const;
    qreal positionX() const;
    qreal positionY() const;

    bool isValid() const;

signals:
    void anchorXChanged();
    void anchorYChanged();
    void positionXChanged();
    void positionYChanged();
    void validChanged();

private:
    friend class CMCanvasControllerDeclarative;
    void setAnchorX(qreal x);
    void setAnchorY(qreal y);
    void setPositionX(qreal x);
    void setPositionY(qreal y);
    void setTextCursor(const QTextCursor &c);
    QTextCursor textCursor() const;

    QTextCursor m_textCursor;
    qreal m_anchorX, m_anchorY, m_positionX, m_positionY;
};

#endif // CALLIGRAMOBILE_TEXTCURSOR_H

