#include "CMMainWindow.h"

#include <QtDeclarative/QDeclarativeView>
#include <QtDeclarative/QDeclarativeEngine>
#include <QtDeclarative/QDeclarativeContext>
#include <QtDeclarative/qdeclarative.h>

#ifdef WITH_QT_SYSTEMINFO
#include <QtCore/QTimer>
#include <QtSystemInfo/QSystemDeviceInfo>
#endif //WITH_QT_SYSTEMINFO

#include <KDE/KStandardDirs>
#include <KDE/KCmdLineArgs>
#include <KDE/KUrl>

#include <kofficeversion.h>

#include "CMIconImageProvider.h"
#include "CMViewStack.h"
#include "CMCanvasControllerDeclarative.h"
#include "CMRecentFilesModel.h"
#include "CMFileSystemModel.h"
#include <KMessageBox>

class CMMainWindow::Private
{
public:
    Private() { }
    ~Private() { }

    QDeclarativeView* view;
    QDeclarativeEngine* engine;

    QStringList recentFiles;
};

CMMainWindow::CMMainWindow( const QString &ui, const QString &file, QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags), d(new Private)
{
#ifdef WITH_QT_SYSTEMINFO
    QTimer::singleShot(0, this, SLOT(checkMultiTouch()));
#endif //WITH_QT_SYSTEMINFO
    
    d->view = new QDeclarativeView(this);
    d->engine = d->view->engine();

    QStringList dataPaths = KGlobal::dirs()->findDirs("data", "calligramobile");
    foreach(const QString& path, dataPaths) {
        d->engine->addImportPath(path);
    }
    d->engine->addImageProvider("icon", new CMIconImageProvider);
    qmlRegisterType<CMViewStack>("org.calligra.mobile", 1, 0, "ViewStack");
    qmlRegisterType<CMCanvasControllerDeclarative>("org.calligra.mobile", 1, 0, "CanvasControllerDeclarative");
    qmlRegisterType<CMRecentFilesModel>("org.calligra.mobile", 1, 0, "RecentFilesModel");
    qmlRegisterType<CMFileSystemModel>("org.calligra.mobile", 1, 0, "FileSystemModel");
    d->view->rootContext()->setContextProperty("KOFFICE_VERSION_STRING", KOFFICE_VERSION_STRING);
    d->view->rootContext()->setContextProperty("window", this);

    d->view->setSource(QUrl(ui));
    d->view->setResizeMode(QDeclarativeView::SizeRootObjectToView);
    d->view->viewport()->grabGesture(Qt::PinchGesture);

    setCentralWidget(d->view);

    if(!file.isEmpty()) {
        qDebug() << "Opening" << QDir::cleanPath(QDir::current().absoluteFilePath(file));
        d->view->rootObject()->setProperty("file", QDir::cleanPath(QDir::current().absoluteFilePath(file)));
        QDeclarativeExpression expr(d->view->rootContext(), d->view->rootObject(), QString("openFile()"));
        expr.evaluate();
        if(expr.hasError())
            qDebug() << expr.error();
    }
}

CMMainWindow::~CMMainWindow()
{
    delete d;
}

void CMMainWindow::toggleFullScreen()
{
    setWindowState(windowState() ^ Qt::WindowFullScreen);
}

void CMMainWindow::checkMultiTouch()
{
#ifdef WITH_QT_SYSTEMINFO
    QtMobility::QSystemDeviceInfo info;
    if(!(info.inputMethodType() & QtMobility::QSystemDeviceInfo::MultiTouch)) {
        qWarning() << "No multi touch available!";
        //if(KMessageBox::warningContinueCancel(this, "No multi-touch available. User experience will be suboptimal.", "No Multi-Touch") != KMessageBox::Continue) {
        //    QApplication::quit();
        //}
    }
#endif
}
