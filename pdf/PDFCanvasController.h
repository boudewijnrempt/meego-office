#ifndef PDFCANVASCONTROLLER_H
#define PDFCANVASCONTROLLER_H

#include <CMCanvasControllerDeclarative.h>


class PDFCanvasController : public CMCanvasControllerDeclarative
{
Q_OBJECT
public:
    PDFCanvasController ( QDeclarativeItem* parent = 0 );
    virtual ~PDFCanvasController();

public Q_SLOTS:
    void loadDocument();

private:
    class Private;
    const QScopedPointer<Private> d;
};

#endif // PDFCANVASCONTROLLER_H
