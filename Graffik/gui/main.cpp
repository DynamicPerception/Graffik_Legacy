#include <QtGui/QApplication>
#include <QDebug>


#include "mainwindow.h"

#include "MoCoBus/ommocobus.h"
#include "MoCoBus/omcommandbuffer.h"
#include "Devices/nanoMoCo/omaxis.h"
#include "MoCoBus/omnetwork.h"

int main(int argc, char *argv[])
{

   // put dlls in a nice place on windows



    QApplication a(argc, argv);

#ifdef Q_OS_WIN32
    QApplication::addLibraryPath(QCoreApplication::applicationDirPath() + "/libs");
#endif

    MainWindow w;
    w.show();

    return a.exec();
}




