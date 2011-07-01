#ifndef CALLIGRAMOBILE_MAINWINDOW_H
#define CALLIGRAMOBILE_MAINWINDOW_H

#ifdef __x86_64__
#include </opt/arch32/usr/include/meegoqmllauncher/launcherwindow.h>
#else
#include <meegoqmllauncher/launcherwindow.h>
#endif
#include "calligramobile_export.h"

class CALLIGRAMOBILE_EXPORT CMMainWindow : public LauncherWindow
{
    Q_OBJECT

public:
    explicit CMMainWindow( const QString &ui, const QString &file = QString(), QWidget *parent = 0);
    virtual ~CMMainWindow();

public Q_SLOTS:
    void openUrl(const QString &url);

private:
    class Private;
    Private * const d;
};

#endif // CALLIGRAMOBILE_MAINWINDOW_H
