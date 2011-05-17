#include "PdfServerTest.h"

#include <QDebug>
#include <QProcess>
#include <QTest>
#include <QCoreApplication>

void PdfServerTest::testInstantiation()
{
    // Start the server process
    QProcess process;
    QStringList arguments;
    arguments << "424242";
    process.start(PDF_SERVER_EXECUTABLE, arguments);
    process.waitForStarted();
    process.close();
    Q_ASSERT(process.exitCode() == 0);

}

QTEST_MAIN(PdfServerTest)
