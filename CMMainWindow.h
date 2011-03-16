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

    bool isFullScreen() const { return QMainWindow::isFullScreen(); }
    void setFullScreen(bool f) { setWindowState(f ? windowState() | Qt::WindowFullScreen : windowState() & ~Qt::WindowFullScreen); }

    int width() const { return QMainWindow::width(); }

    int height() const { return QMainWindow::height(); }

    Q_INVOKABLE void openUrl(const QString &url);

protected:
    void changeEvent(QEvent *event);
    void resizeEvent(QResizeEvent *event);

signals:
    void fullScreenChanged();
    void widthChanged();
    void heightChanged();

private:
    class Private;
    Private * const d;

private slots:
    void checkMultiTouch();
};

#endif // CALLIGRAMOBILE_MAINWINDOW_H
