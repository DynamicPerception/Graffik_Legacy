#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <QDebug>
#include <QFileDialog>

#include "Slim/SlimFileHandler/slimfilehandler.h"
#include "docs/helpwindow.h"
#include "core/DeviceScan/devicescanner.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _net = new OMNetwork();
    _axisOpts = new AxisOptions(this);
    _netModel = new NetworkModel(_net, this);
    _parser = new SlimCommandParser(_net);
    _cmdHist = new CommandHistoryModel(_net, this);
    _filmWindow = new FilmWindow(_net, _axisOpts, this);
    _slimWindow =  new SlimWindow(_net, _cmdHist, _parser, ui->tabs);
    _uData = new UserData(this);

    ui->tabs->addTab(_filmWindow, "Film");
    ui->tabs->addTab(_slimWindow, "Slim");

        // make sure that the slim parser can update its alias and registration when a new device is added/deleted
        // (must be done before recoverBuses)
    QObject::connect(_net, SIGNAL(deviceAdded(OMbusInfo*,OMdeviceInfo*)), _slimWindow, SLOT(registerNewDevice(OMbusInfo*,OMdeviceInfo*)));
    QObject::connect(_net, SIGNAL(deviceDeleted(OMbusInfo*,unsigned short)), _slimWindow, SLOT(removeDevice(OMbusInfo*,unsigned short)));

        // create new options for devices as they are added/deleted
    QObject::connect(_net, SIGNAL(deviceAdded(OMbusInfo*,OMdeviceInfo*)), _axisOpts, SLOT(deviceAdded(OMbusInfo*,OMdeviceInfo*)));
    QObject::connect(_net, SIGNAL(deviceDeleted(OMbusInfo*,unsigned short)), _axisOpts, SLOT(deviceDeleted(OMbusInfo*,unsigned short)));

        // recover stored bus data back to network manager
        // Do this BEFORE connecting up the signals and slots in uData to
        // avoid wasting time writing back the same data to the
        // ini file.  This recovers all buses and devices
    _uData->recoverBuses(_net);

        // always recover axis options after recovering buses (so devices will have been created already)
        // doing this in the reverse order will result in all devices getting created and then their options
        // overwritten with the default values
    _uData->recoverAxisOptions(_axisOpts);

        // connect network manager to userdata handlers (to save input information automatically)
    QObject::connect(_net, SIGNAL(busAdded(OMbusInfo*)), _uData, SLOT(busAdded(OMbusInfo*)));
    QObject::connect(_net, SIGNAL(busDeleted(QString,QString)), _uData, SLOT(busDeleted(QString,QString)));
    QObject::connect(_net, SIGNAL(deviceAdded(OMbusInfo*,OMdeviceInfo*)), _uData, SLOT(deviceAdded(OMbusInfo*,OMdeviceInfo*)));
    QObject::connect(_net, SIGNAL(deviceDeleted(OMbusInfo*,unsigned short)), _uData, SLOT(deviceDeleted(OMbusInfo*,unsigned short)));

        // again, we connect the userdata to the OMAxisFilmOptions object -after- recoverBuses to prevent the loading
        // of stored axis options to trigger a re-write back to the ini file.  We do not use a queued connection here.
    QObject::connect(_axisOpts, SIGNAL(deviceOptionsChanged(OMaxisOptions*,unsigned short)), _uData, SLOT(deviceOptionsChanged(OMaxisOptions*,unsigned short)));
    QObject::connect(_axisOpts, SIGNAL(deviceOptionsRemoved(unsigned short)), _uData, SLOT(deviceOptionsRemoved(unsigned short)));


}

MainWindow::~MainWindow()
{

        // ALWAYS delete userdata first to prevent it catching
        // signals as other classes are destroyed

    delete _uData;


    delete _filmWindow;
    delete _slimWindow;
    delete _parser;
    delete _cmdHist;
    delete _netModel;
    delete _axisOpts;
    delete _net;
    delete ui;

}

void MainWindow::on_actionAdd_Bus_triggered() {
    AddNetDialog addNet(_net, this);
    addNet.exec();
}

void MainWindow::on_actionAdd_Device_triggered() {
    AddDeviceDialog addDev(_net, this);
    addDev.exec();
}


void MainWindow::on_actionOpen_File_triggered() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Script"), "", tr("Slim Scripts (*.slim)"));
    qDebug() << "MW: SlimOpen Got File: " << fileName;
    SlimFileHandler::readFile(fileName, _parser, _cmdHist, true);
}

void MainWindow::on_actionSave_File_triggered() {
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Script"), "", tr("Slim Script (*.slim)"));
    qDebug() << "MW: SlimSave Got File: " << fileName;
    SlimFileHandler::writeFile(fileName, _net, _cmdHist, true);
}


void MainWindow::on_actionManage_Network_triggered() {
    NetworkManager netMan(_netModel, _axisOpts, this);
    netMan.exec();
}

 // TODO: Fix leak
void MainWindow::on_actionHelp_Contents_triggered() {
    qDebug() << "MW: Help Index triggered";
    HelpWindow* help = new HelpWindow(this);
    help->show();
}

 // TODO: Fix leak
void MainWindow::on_actionScan_for_Devices_triggered() {
    qDebug() << "MW: Scan for Devices Triggered";
    DeviceScanner* scan = new DeviceScanner(_net);
}

void MainWindow::on_actionInitialize_New_Device_triggered() {
    qDebug() << "MW: Initialize New Device Triggered";
    DeviceScanner* scan = new DeviceScanner(_net, 2);
}

/*void MainWindow::setSlimWindow(SlimWindow * p_slim) {
    _slimWindow = p_slim;
    QObject::connect(_slimWindow, SIGNAL(out()), this, SLOT(slimClose()), Qt::QueuedConnection);

}

void MainWindow::on_slimButton_clicked() {
    this->hide();
    _slimWindow->show();
}
*/
