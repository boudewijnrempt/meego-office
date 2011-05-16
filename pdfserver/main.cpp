#include <qtsinglecoreapplication.h>

// Single instance
int main(int argc, char **argv)
{
     QtSingleCoreApplication app(argc, argv);

     if (app.isRunning())
         return 0;

     return app.exec();
 }
