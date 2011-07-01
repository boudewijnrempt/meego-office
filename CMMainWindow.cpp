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

};

CMMainWindow::CMMainWindow( const QString &ui, const QString &file, QWidget *parent)
    : LauncherWindow(true, 1280, 800, false, false, parent), d(new Private)
{
    setWindowTitle(i18n("MeeGo Office Suite"));

    QStringList dataPaths = KGlobal::dirs()->findDirs("data", "meego-office-suite");
    qDebug() << dataPaths;
    foreach(const QString& path, dataPaths) {
        engine()->addImportPath(path);
    }
    engine()->addImageProvider("icon", new CMIconImageProvider);
    rootContext()->setContextProperty("CALLIGRA_VERSION_STRING", CALLIGRA_VERSION_STRING);

    setSource(QUrl(ui));
    setResizeMode(QDeclarativeView::SizeRootObjectToView);
    viewport()->grabGesture(Qt::PinchGesture);
    viewport()->grabGesture(Qt::SwipeGesture);

    CMPageThumbnailProvider* thumbProvider = new CMPageThumbnailProvider();
    engine()->addImageProvider("pagethumbnails", thumbProvider);

    if (!file.isEmpty()) {
        QString cleanFileName = QDir::cleanPath(QDir::current().absoluteFilePath(file));

        QString ext = file.right(file.size() - (file.lastIndexOf('.') + 1));
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

        QDeclarativeExpression expr(rootContext(), rootObject(), QString("pageStack.currentPage.openFile('%1','%2')").arg(cleanFileName, fileType));
        expr.evaluate();
        if(expr.hasError())
            qDebug() << expr.error();
    }
}

CMMainWindow::~CMMainWindow()
{
    delete d;
}

void CMMainWindow::openUrl(const QString &url)
{
    QDesktopServices::openUrl(url);
}

#include "CMMainWindow.moc"
