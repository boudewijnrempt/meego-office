#ifndef CMTEXTSELECTION_H
#define CMTEXTSELECTION_H

#include <QtGui/QTextCursor>

class KoTextShapeData;
class CMCanvasControllerDeclarative;
class QPointF;
class QDeclarativeItem;

class CMTextSelection
{

public:
    CMTextSelection(CMCanvasControllerDeclarative *controller);
    virtual ~CMTextSelection();

    enum UpdateWhat
    {
        UpdatePosition,
        UpdateAnchor
    };

    void setAnchorHandle(QDeclarativeItem *anchor);
    void setPositionHandle(QDeclarativeItem *position);

    bool hasSelection() const;

    virtual void copyText();

    virtual void updateFromHandles();
    virtual void updateHandlePositions(const QTextCursor &cursor = QTextCursor());
    virtual void updatePosition(UpdateWhat update, const QPointF &position);

    virtual QPointF documentToView(const QPointF& point) = 0;
    virtual QPointF viewToDocument(const QPointF& point) = 0;

    virtual KoTextShapeData *textShapeDataForPosition(const QPointF &position);

protected:
    void setHasSelection(bool selection);

private:
    class Private;
    Private * const d;
};

#endif // CMTEXTSELECTION_H
