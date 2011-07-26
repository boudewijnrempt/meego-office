#ifndef CALLIGRAMOBILE_MAINWINDOW_H
#define CALLIGRAMOBILE_MAINWINDOW_H

#ifdef USE_MEEGO_QMLLAUNCHER
#include <meegoqmllauncher/launcherwindow.h>

class MainWindow : public LauncherWindow
{
#else
#include <QtDeclarative/QDeclarativeView>

class MainWindow : public QDeclarativeView
{
#endif
    Q_OBJECT

public:
    explicit MainWindow( const QString &ui, const QString &file = QString(), QWidget *parent = 0);
    virtual ~MainWindow();

private:
    class Private;
    Private * const d;
};

#endif // CALLIGRAMOBILE_MAINWINDOW_H
