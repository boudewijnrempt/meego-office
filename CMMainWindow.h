#ifndef CALLIGRAMOBILE_MAINWINDOW_H
#define CALLIGRAMOBILE_MAINWINDOW_H

#include <QtGui/QMainWindow>
#include "calligramobile_export.h"

class CALLIGRAMOBILE_EXPORT CMMainWindow : public QMainWindow
{
    Q_OBJECT
    Q_PROPERTY(bool fullScreen READ isFullScreen WRITE setFullScreen NOTIFY fullScreenChanged)

public:
    explicit CMMainWindow( const QString &ui, const QString &file = QString(), QWidget *parent = 0, Qt::WindowFlags flags = 0);
    virtual ~CMMainWindow();

    bool isFullScreen() const { return QMainWindow::isFullScreen(); }
    void setFullScreen(bool f) { setWindowState(f ? windowState() | Qt::WindowFullScreen : windowState() & ~Qt::WindowFullScreen); }

protected:
    void changeEvent(QEvent *event);

signals:
    void fullScreenChanged();

private:
    class Private;
    Private * const d;

private slots:
    void checkMultiTouch();
};

#endif // CALLIGRAMOBILE_MAINWINDOW_H
