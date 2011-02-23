#include <QtCore/QDebug>
#include <QtDeclarative/qdeclarative.h>

#include <KDE/KApplication>
#include <KDE/KAboutData>
#include <KDE/KCmdLineArgs>
#include <KDE/KStandardDirs>
#include <KDE/KComponentData>

#include "../CMMainWindow.h"
#include "CMTablesCanvas.h"

int main(int argc, char *argv[])
{
    QApplication::setGraphicsSystem("raster");
    
    KAboutData aboutData(
        "calligratables_mobile",
        0,
        ki18n("Calligra Tables"),
        "0.1",
        ki18n("Mobile version of Calligra Tables"),
        KAboutData::License_GPL,
        ki18n("(c) 2010"),
        ki18n("Some text..."),
        "http://www.calligra-suite.com/",
        "calligra@bugs.kde.org");

    KCmdLineArgs::init( argc, argv, &aboutData );
    KCmdLineOptions options;
    options.add( "+[file]", ki18n( "Document to open (.odt or .doc)" ) );
    KCmdLineArgs::addCmdLineOptions( options );

    KCmdLineArgs* args = KCmdLineArgs::parsedArgs();
    QString fileName;
    if(args->count() > 0)
        fileName = args->arg(0);
    
    KApplication app;

    qmlRegisterType<CMTablesCanvas>("org.calligra.mobile", 1, 0, "TablesCanvas");

    CMMainWindow window(KStandardDirs::locate("appdata", "Tables.qml"), fileName);
    window.show();

    return app.exec();
}
