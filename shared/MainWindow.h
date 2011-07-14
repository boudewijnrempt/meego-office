#ifndef CALLIGRAMOBILE_MAINWINDOW_H
#define CALLIGRAMOBILE_MAINWINDOW_H

#ifdef __x86_64__
#include </opt/arch32/usr/include/meegoqmllauncher/launcherwindow.h>
#else
#include <meegoqmllauncher/launcherwindow.h>
#endif

class MainWindow : public LauncherWindow
{
    Q_OBJECT

public:
    explicit MainWindow( const QString &ui, const QString &file = QString(), QWidget *parent = 0);
    virtual ~MainWindow();

private:
    class Private;
    Private * const d;
};

#endif // CALLIGRAMOBILE_MAINWINDOW_H
