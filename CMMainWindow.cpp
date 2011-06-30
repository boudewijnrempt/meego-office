#include "CMMainWindow.h"

#include <QtDeclarative>

#include <QDir>

#ifdef WITH_QT_SYSTEMINFO
#include <QtCore/QTimer>
#include <QtSystemInfo/QSystemDeviceInfo>
#endif //WITH_QT_SYSTEMINFO

#include <KDE/KStandardDirs>
#include <KDE/KCmdLineArgs>
#include <KDE/KUrl>

#include <calligraversion.h>

#include "CMIconImageProvider.h"
#include "CMCanvasControllerDeclarative.h"
#include <KMessageBox>
#include "CMPageThumbnailProvider.h"

class CMMainWindow::Private
{
public:
    Private() { }
    ~Private() { }

    void checkMultiTouch();

    QDeclarativeView* view;
    QDeclarativeEngine* engine;

    QStringList recentFiles;
};

CMMainWindow::CMMainWindow( const QString &ui, const QString &file, QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags), d(new Private)
{
    setWindowTitle(i18n("Calligra Mobile"));

#ifdef WITH_QT_SYSTEMINFO
    QTimer::singleShot(0, this, SLOT(checkMultiTouch()));
#endif //WITH_QT_SYSTEMINFO

    d->view = new QDeclarativeView(this);
    d->engine = d->view->engine();

    QStringList dataPaths = KGlobal::dirs()->findDirs("data", "meego-office-suite");
    foreach(const QString& path, dataPaths) {
        d->engine->addImportPath(path);
    }
    d->engine->addImageProvider("icon", new CMIconImageProvider);
    d->view->rootContext()->setContextProperty("CALLIGRA_VERSION_STRING", CALLIGRA_VERSION_STRING);
    d->view->rootContext()->setContextProperty("qApp", qApp);
    d->view->rootContext()->setContextProperty("mainWindow", this);

    d->view->setSource(QUrl(ui));
    d->view->setResizeMode(QDeclarativeView::SizeRootObjectToView);
    d->view->viewport()->grabGesture(Qt::PinchGesture);
    d->view->viewport()->grabGesture(Qt::SwipeGesture);

    CMPageThumbnailProvider* thumbProvider = new CMPageThumbnailProvider();
    d->engine->addImageProvider("pagethumbnails", thumbProvider);

    setCentralWidget(d->view);

    if (!file.isEmpty()) {
        QString cleanFileName = QDir::cleanPath(QDir::current().absoluteFilePath(file));

        QString ext = file.right(file.size() - (file.lastIndexOf('.') + 1));
        qDebug() << "Ext:" << ext;
        QString docTypesText[] = { tr("Text Document"), tr("Presentation"), tr("Spreadsheet") };
        QString fileType;
        if(ext == "doc" || ext == "docx" || ext == "odt") {
            fileType = docTypesText[0];
        } else if(ext == "xls" || ext == "xlsx" || ext == "ods") {
            fileType = docTypesText[1];
        } else if(ext == "ppt" || ext == "pptx" || ext == "odp") {
            fileType = docTypesText[2];
        } else {
            return;
        }

        QDeclarativeExpression expr(d->view->rootContext(), d->view->rootObject(), QString("pageStack.currentPage.openFile('%1','%2')").arg(cleanFileName, fileType));
        expr.evaluate();
        if(expr.hasError())
            qDebug() << expr.error();
    }
}

CMMainWindow::~CMMainWindow()
{
    delete d;
}

void CMMainWindow::resizeEvent(QResizeEvent *event)
{
    if (event->oldSize().width() != width())
        emit widthChanged();
    if (event->oldSize().height() != height())
        emit heightChanged();
    QMainWindow::resizeEvent(event);
}

void CMMainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::WindowStateChange) {
        QWindowStateChangeEvent *ev = static_cast<QWindowStateChangeEvent *>(event);
        if ((ev->oldState() & Qt::WindowFullScreen) != (windowState() & Qt::WindowFullScreen))
            emit fullScreenChanged();
    }
    QMainWindow::changeEvent(event);
}

void CMMainWindow::openUrl(const QString &url)
{
    QDesktopServices::openUrl(url);
}

bool CMMainWindow::isFullScreen() const
{
    return windowState() & Qt::WindowFullScreen;
}

void CMMainWindow::setFullScreen(bool fullScreen)
{
    if(fullScreen) {
        setWindowState(windowState() | Qt::WindowFullScreen);
    } else {
        setWindowState(windowState() & ~Qt::WindowFullScreen);
    }
}

void CMMainWindow::Private::checkMultiTouch()
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

#include "CMMainWindow.moc"
