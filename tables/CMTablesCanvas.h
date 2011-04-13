#ifndef CALLIGRAMOBILE_TABLESCANVAS_H
#define CALLIGRAMOBILE_TABLESCANVAS_H

#include "../CMCanvasControllerDeclarative.h"

class CMTablesCanvas : public CMCanvasControllerDeclarative
{
    Q_OBJECT
    Q_PROPERTY(int activeSheetIndex READ activeSheetIndex WRITE setActiveSheetIndex)
    Q_PROPERTY(QObject* document READ doc)
    Q_PROPERTY(bool hasNextSheet READ hasNextSheet NOTIFY hasNextSheetChanged)
    Q_PROPERTY(bool hasPreviousSheet READ hasNextSheet NOTIFY hasPreviousSheetChanged)
    Q_PROPERTY(int sheet READ activeSheetIndex WRITE changeSheet NOTIFY sheetChanged)
    Q_PROPERTY(QString sheetName READ sheetName)

public:
    explicit CMTablesCanvas(QDeclarativeItem* parent = 0);
    virtual ~CMTablesCanvas();

    int activeSheetIndex() const;
    void setActiveSheetIndex(int index);

    bool hasNextSheet() const;
    bool hasPreviousSheet() const;

    QString sheetName() const;

    QObject* doc() const;

public Q_SLOTS:
    void nextSheet();
    void previousSheet();
    void changeSheet(int newIndex);
    void loadDocument();

signals:
    void hasNextSheetChanged();
    void hasPreviousSheetChanged();
    void sheetChanged(int newIndex);

protected:

private:
    class Private;
    Private * const d;

private Q_SLOTS:
    void updateDocumentSizePrivate(const QSize& size);
};

#endif // CALLIGRAMOBILE_TABLESCANVAS_H
