#include "qtsinglecoreapplication.h"
#include "PdfServer.h"

#include <QString>
#include <QStringList>
#include <QHostAddress>

// Single instance
int main(int argc, char **argv)
{
     QtSingleCoreApplication app(argc, argv);

     if (app.isRunning())
         return 0;

     // get the port number
     bool result;
     int port = app.arguments().at(1).toInt(&result);
     if (!result)
         return 1; // did not get a port argument

     // create the server, using the first localhost address
     PdfServer psdServer;
     if (!psdServer.listen(QHostAddress::LocalHost, port)) {
         return 2; // could not start to listen
     }



     return app.exec();
 }
