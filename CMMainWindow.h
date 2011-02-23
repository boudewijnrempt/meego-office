#ifndef CALLIGRAMOBILE_MAINWINDOW_H
#define CALLIGRAMOBILE_MAINWINDOW_H

#include <QtGui/QMainWindow>
#include "calligramobile_export.h"

class CALLIGRAMOBILE_EXPORT CMMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit CMMainWindow( const QString &ui, const QString &file = QString(), QWidget *parent = 0, Qt::WindowFlags flags = 0);
    virtual ~CMMainWindow();

private:
    class Private;
    Private * const d;

private slots:
    void checkMultiTouch();
};

#endif // CALLIGRAMOBILE_MAINWINDOW_H
