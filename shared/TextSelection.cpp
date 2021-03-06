#include "TextSelection.h"

#include <QtCore/QBuffer>
#include <QtCore/QMimeData>
#include <QtGui/QTextDocumentFragment>
#include <QtGui/QTextDocumentWriter>
#include <QtGui/QApplication>
#include <QtDeclarative/QDeclarativeItem>

#include <KoCanvasBase.h>
#include <KoShape.h>
#include <KoShapeManager.h>
#include <KoTextEditor.h>
#include <KoTextShapeData.h>
#include <KoTextLayoutRootArea.h>

#include "CanvasControllerDeclarative.h"

class TextSelection::Private
{
public:
    Private( TextSelection *qq)
        : q(qq),
          anchorHandle(0),
          positionHandle(0),
          updateHandles(true),
          hasSelection(false)
    { }

    TextSelection *q;

    QDeclarativeItem *anchorHandle;
    QDeclarativeItem *positionHandle;

    CanvasControllerDeclarative *controller;

    QTextCursor selection;

    bool updateHandles;
    bool hasSelection;
};

TextSelection::TextSelection( CanvasControllerDeclarative* controller)
    : d(new Private(this))
{
    d->controller = controller;
}

TextSelection::~TextSelection()
{
    delete d;
}

void TextSelection::setAnchorHandle(QDeclarativeItem* anchor)
{
    d->anchorHandle = anchor;
}

void TextSelection::setPositionHandle(QDeclarativeItem* position)
{
    d->positionHandle = position;
}

bool TextSelection::hasSelection() const
{
    return d->hasSelection;
}

void TextSelection::copySelection()
{
    QMimeData *mimeData = new QMimeData;
    QTextDocumentFragment fragment(d->selection);
    mimeData->setText(fragment.toPlainText());
    mimeData->setHtml(fragment.toHtml("utf-8"));
    QBuffer buffer;
    QTextDocumentWriter writer(&buffer, "ODF");
    writer.write(fragment);
    buffer.close();
    mimeData->setData("application/vnd.oasis.opendocument.text", buffer.data());
    QApplication::clipboard()->setMimeData(mimeData);
}

void TextSelection::updateFromHandles()
{
    if(!d->anchorHandle || !d->positionHandle) {
        return;
    }

    if(d->anchorHandle->isVisible() && d->positionHandle->isVisible()) {
        d->updateHandles = false;
        updatePosition(UpdateAnchor, QPointF(d->anchorHandle->x(), d->anchorHandle->y()));
        updatePosition(UpdatePosition, QPointF(d->positionHandle->x(), d->positionHandle->y()));
        d->updateHandles = true;
    }
    updateHandlePositions();
}

void TextSelection::updateHandlePositions(const QTextCursor &cursor)
{
    if(!d->anchorHandle || !d->positionHandle || d->selection.isNull() || !d->selection.hasSelection()) {
        return;
    }

    if(!cursor.isNull()) {
        d->selection = cursor;
    }

    KoTextDocumentLayout * layout = qobject_cast<KoTextDocumentLayout*>(d->selection.document()->documentLayout());

    QTextCursor positionCursor = QTextCursor(d->selection.document());
    positionCursor.setPosition(d->selection.position());
    QTextLine line = positionCursor.block().layout()->lineForTextPosition(positionCursor.positionInBlock());
    if(line.isValid()) {
        KoShape *shape = layout->rootAreaForPosition(d->selection.position())->associatedShape();
        KoTextShapeData *shapeData = qobject_cast<KoTextShapeData *>(shape->userData());

        QPointF pos = shape->shapeToDocument(layout->selectionBoundingBox(positionCursor).center());
        pos = documentToView(pos) - d->controller->getDocumentOffset();

        d->positionHandle->blockSignals(true);
        d->positionHandle->setPos(pos);
        d->positionHandle->blockSignals(false);
        d->positionHandle->setVisible(true);
    }

    QTextCursor anchorCursor = QTextCursor(d->selection.document());
    anchorCursor.setPosition(d->selection.anchor());
    line = anchorCursor.block().layout()->lineForTextPosition(anchorCursor.positionInBlock());
    if(line.isValid()) {
        KoShape *shape = layout->rootAreaForPosition(anchorCursor.position())->associatedShape();
        KoTextShapeData *shapeData = qobject_cast<KoTextShapeData *>(shape->userData());

        QPointF pos = shape->shapeToDocument(layout->selectionBoundingBox(anchorCursor).center());
        pos = documentToView(pos) - d->controller->getDocumentOffset();

        d->anchorHandle->blockSignals(true);
        d->anchorHandle->setPos(pos);
        d->anchorHandle->blockSignals(false);
        d->anchorHandle->setVisible(true);
    }
}

void TextSelection::updatePosition( TextSelection::UpdateWhat update, const QPointF& position)
{
    KoTextShapeData * shapeData = textShapeDataForPosition(position + d->controller->getDocumentOffset());
    if(!shapeData) {
        return;
    }

    QPointF docPos = viewToDocument(position + d->controller->getDocumentOffset());
    QPointF shapeMousePos = shapeData->rootArea()->associatedShape()->absoluteTransformation(0).inverted().map(docPos);
    QPointF textDocMousePos = shapeMousePos + QPointF(0.0, shapeData->documentOffset());

    int cursorPos = shapeData->rootArea()->hitTest(textDocMousePos, Qt::FuzzyHit).position;
    KoTextEditor *editor = KoTextDocument(shapeData->document()).textEditor();
    if(update == UpdatePosition) {
        editor->setPosition(cursorPos, QTextCursor::KeepAnchor);
    } else {
        editor->setPosition(cursorPos, QTextCursor::MoveAnchor);
    }

    d->selection = *(editor->cursor());

    if(editor->hasSelection()) {
        d->controller->canvas()->updateCanvas(shapeData->rootArea()->associatedShape()->boundingRect());
        d->hasSelection = true;
        if(d->updateHandles) {
            updateHandlePositions();
        }
    } else {
        d->hasSelection = false;
    }
}

KoTextShapeData* TextSelection::textShapeDataForPosition(const QPointF& position)
{
    QPointF docMousePos = viewToDocument(position);
    KoShape *shapeUnderCursor = d->controller->canvas()->shapeManager()->shapeAt(docMousePos);
    if(!shapeUnderCursor) {
        return 0;
    }
    KoTextShapeData *shapeData = qobject_cast<KoTextShapeData *>(shapeUnderCursor->userData());
    if (!shapeData) {
        return 0;
    }

    return shapeData;
}

void TextSelection::setHasSelection(bool selection)
{
    d->hasSelection = selection;
    d->selection.clearSelection();
}

