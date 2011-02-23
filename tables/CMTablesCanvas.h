#ifndef CALLIGRAMOBILE_TABLESCANVAS_H
#define CALLIGRAMOBILE_TABLESCANVAS_H

#include "../CMCanvasControllerDeclarative.h"

class CMTablesCanvas : public CMCanvasControllerDeclarative
{
    Q_OBJECT
    Q_PROPERTY(int activeSheetIndex READ activeSheetIndex WRITE setActiveSheetIndex)
    Q_PROPERTY(QObject* document READ doc)
    Q_PROPERTY(QString file READ file WRITE setFile)
    Q_PROPERTY(bool hasNextSheet READ hasNextSheet NOTIFY hasNextSheetChanged)
    Q_PROPERTY(bool hasPreviousSheet READ hasNextSheet NOTIFY hasPreviousSheetChanged)

public:
    explicit CMTablesCanvas(QDeclarativeItem* parent = 0);
    virtual ~CMTablesCanvas();

    int activeSheetIndex() const;
    void setActiveSheetIndex(int index);

    bool hasNextSheet() const;
    bool hasPreviousSheet() const;

    QObject* doc() const;
    QString file() const;
    void setFile(const QString &file);

public Q_SLOTS:
    void nextSheet();
    void previousSheet();

signals:
    void hasNextSheetChanged();
    void hasPreviousSheetChanged();

protected:

private:
    class Private;
    Private * const d;

private Q_SLOTS:
    void openFile();

    void updateDocumentSizePrivate(const QSize& size);
};

#endif // CALLIGRAMOBILE_TABLESCANVAS_H
