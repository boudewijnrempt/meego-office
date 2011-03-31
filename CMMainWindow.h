#ifndef CALLIGRAMOBILE_MAINWINDOW_H
#define CALLIGRAMOBILE_MAINWINDOW_H

#include <QtGui/QMainWindow>
#include "calligramobile_export.h"

class CALLIGRAMOBILE_EXPORT CMMainWindow : public QMainWindow
{
    Q_OBJECT
    Q_PROPERTY(bool fullScreen READ isFullScreen WRITE setFullScreen NOTIFY fullScreenChanged)
    Q_PROPERTY(int width READ width NOTIFY widthChanged)
    Q_PROPERTY(int height READ height NOTIFY heightChanged)

public:
    explicit CMMainWindow( const QString &ui, const QString &file = QString(), QWidget *parent = 0, Qt::WindowFlags flags = 0);
    virtual ~CMMainWindow();

    Q_INVOKABLE bool isFullScreen() const;

public Q_SLOTS:
    void setFullScreen(bool fullScreen);
    void openUrl(const QString &url);

protected:
    void resizeEvent(QResizeEvent *event);
    void changeEvent(QEvent *event);

Q_SIGNALS:
    void fullScreenChanged();
    void widthChanged();
    void heightChanged();

private:
    class Private;
    Private * const d;

    Q_PRIVATE_SLOT(d, void checkMultiTouch());
};

#endif // CALLIGRAMOBILE_MAINWINDOW_H
