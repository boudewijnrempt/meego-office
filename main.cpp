#include <QtCore/QDebug>
#include <QtDeclarative/qdeclarative.h>

#include <KDE/KAboutData>
#include <KDE/KCmdLineArgs>
#include <KDE/KStandardDirs>
#include <KDE/KComponentData>

#ifdef __x86_64__
#include </opt/arch32/usr/include/meegoqmllauncher/launcherapp.h>
#else
#include <meegoqmllauncher/launcherapp.h>
#endif

#include "shared/MainWindow.h"
#include "shared/DocumentListModel.h"
#include "shared/DocumentThumbnailListModel.h"

#include "words/WordsCanvas.h"
#include "tables/TablesCanvas.h"
#include "tables/TablesHeader.h"
#include "stage/StageCanvas.h"
#include "pdf/PDFCanvasController.h"

int main(int argc, char *argv[])
{
    QApplication::setGraphicsSystem("opengl");

    KAboutData aboutData(
        "meego-office-suite",
        0,
        ki18n("MeeGo Office Suite"),
        "0.1",
        ki18n("Office Suite for MeeGo, Developed by Intel"),
        KAboutData::License_Unknown,
        ki18n("(c) 2010"),
        ki18n("Some text..."),
        "http://www.kogmbh.com/",
        "team-intel@kogmbh.com");

    KCmdLineArgs::init( argc, argv, &aboutData );
    KCmdLineOptions options;
    options.add( "+[file]", ki18n( "Document to open" ) );
    KCmdLineArgs::addCmdLineOptions( options );

    KCmdLineArgs* args = KCmdLineArgs::parsedArgs();
    QString fileName;
    if(args->count() > 0) {
        fileName = args->arg(0);
    }
    
    LauncherApp app(argc, argv);
 
    qmlRegisterType<DocumentListModel>("org.calligra.mobile", 1, 0, "DocumentListModel");
    qmlRegisterType<DocumentThumbnailListModel>("org.calligra.mobile", 1, 0, "DocumentThumbnailListModel");
    qmlRegisterType<WordsCanvas>("org.calligra.mobile", 1, 0, "WordsCanvas");
    qmlRegisterType<TablesCanvas>("org.calligra.mobile", 1, 0, "TablesCanvas");
    qmlRegisterType<TablesHeader>("org.calligra.mobile", 1, 0, "TablesHeader");
    qmlRegisterType<StageCanvas>("org.calligra.mobile", 1, 0, "StageCanvas");
    qmlRegisterType<PDFCanvasController>("org.calligra.mobile", 1, 0, "PDFCanvas");

    MainWindow window(KStandardDirs::locate("data", "meego-office-suite/main.qml"), fileName);
    window.showMaximized();

    return app.exec();
}
