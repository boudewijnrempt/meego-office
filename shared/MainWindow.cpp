#include "MainWindow.h"

#include <QtCore/QDir>
#include <QtDeclarative/QDeclarativeEngine>
#include <QtDeclarative/QDeclarativeContext>
#include <QtDeclarative/QDeclarativeExpression>

#include <KDE/KStandardDirs>
#include <KDE/KCmdLineArgs>
#include <KDE/KUrl>

#include <calligraversion.h>

#include "IconImageProvider.h"
#include "CanvasControllerDeclarative.h"
#include "PageThumbnailProvider.h"
#include "DocumentListModel.h"

class MainWindow::Private
{
public:
    Private() { }
    ~Private() { }

};

MainWindow::MainWindow( const QString &ui, const QString &file, QWidget *parent)
    : LauncherWindow(true, 1280, 800, false, false, parent), d(new Private)
{
    setWindowTitle(tr("MeeGo Office Suite"));

    QStringList dataPaths = KGlobal::dirs()->findDirs("data", "meego-office-suite");
    foreach(const QString& path, dataPaths) {
        engine()->addImportPath(path);
    }
    engine()->addImageProvider("icon", new IconImageProvider);
    engine()->addImageProvider("pagethumbnails", new PageThumbnailProvider());
    rootContext()->setContextProperty("CALLIGRA_VERSION_STRING", CALLIGRA_VERSION_STRING);

    setSource(QUrl(ui));
    setResizeMode(QDeclarativeView::SizeRootObjectToView);
    viewport()->grabGesture(Qt::PinchGesture);
    viewport()->grabGesture(Qt::SwipeGesture);

    if (!file.isEmpty()) {
        QString cleanFileName = QDir::cleanPath(QDir::current().absoluteFilePath(file));

        DocumentListModel::DocumentType type = DocumentListModel::typeForFile(cleanFileName);

        QDeclarativeExpression expr(rootContext(), rootObject(), QString("pageStack.currentPage.openFile('%1','%2')").arg(cleanFileName, type));
        expr.evaluate();
        if(expr.hasError()) {
            qDebug() << expr.error();
        }
    }
}

MainWindow::~MainWindow()
{
    delete d;
}
