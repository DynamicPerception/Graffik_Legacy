#include <QtGui/QApplication>
#include <QDebug>


#include "mainwindow.h"

#include "MoCoBus/ommocobus.h"
#include "MoCoBus/omcommandbuffer.h"
#include "Devices/nanoMoCo/omaxis.h"
#include "MoCoBus/omnetwork.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}




