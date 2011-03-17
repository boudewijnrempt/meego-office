#include "CMTextCursor.h"

CMTextCursor::CMTextCursor()
{
}

CMTextCursor::~CMTextCursor()
{
}

qreal CMTextCursor::anchorX() const
{
    return m_anchorX;
}

void CMTextCursor::setAnchorX(qreal x)
{
    m_anchorX = x;
    emit anchorXChanged();
}

qreal CMTextCursor::anchorY() const
{
    return m_anchorY;
}

void CMTextCursor::setAnchorY(qreal y)
{
    m_anchorY = y;
    emit anchorYChanged();
}

qreal CMTextCursor::positionX() const
{
    return m_positionX;
}

void CMTextCursor::setPositionX(qreal x)
{
    m_positionX = x;
    emit positionXChanged();
}

qreal CMTextCursor::positionY() const
{
    return m_positionY;
}

void CMTextCursor::setPositionY(qreal y)
{
    m_positionY = y;
    emit positionYChanged();
}

QTextCursor CMTextCursor::textCursor() const
{
    return m_textCursor;
}

void CMTextCursor::setTextCursor(const QTextCursor &c)
{
    m_textCursor = c;
    emit validChanged();
}

bool CMTextCursor::isValid() const
{
    return !m_textCursor.isNull();
}

