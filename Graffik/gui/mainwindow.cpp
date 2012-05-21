#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <QDebug>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _net = new OMNetwork();
    _netModel = new networkModel(_net, this);
    _parser = new SlimCommandParser();
    _slimWindow =  new SlimWindow(_net, _parser, ui->tabs);
    _filmWindow = new filmWindow(_net, this);
    _uData = new UserData(this);
    _axisOpts = new OMAxisFilmOptions(this);

    ui->tabs->addTab(_slimWindow, "Slim");
    ui->tabs->addTab(_filmWindow, "Film");

        // make sure that the slim parser can update its alias and registration when a new device is added
        // (must be done before recoverBuses)
    QObject::connect(_net, SIGNAL(deviceAdded(OMbusInfo*,OMdeviceInfo*)), _slimWindow, SLOT(registerNewDevice(OMbusInfo*,OMdeviceInfo*)), Qt::QueuedConnection);

        // create new options for devices as they are added
    QObject::connect(_net, SIGNAL(deviceAdded(OMbusInfo*,OMdeviceInfo*)), _axisOpts, SLOT(deviceAdded(OMbusInfo*,OMdeviceInfo*)), Qt::QueuedConnection);

        // recover stored bus data back to network manager
        // Do this BEFORE connecting up the signals and slots in uData to
        // avoid wasting time writing back the same data to the
        // ini file.  This recovers all buses and devices
    _uData->recoverBuses(_net);

        // connect network manager to userdata handlers (to save input information automatically)
    QObject::connect(_net, SIGNAL(busAdded(OMbusInfo*)), _uData, SLOT(busAdded(OMbusInfo*)), Qt::QueuedConnection);
    QObject::connect(_net, SIGNAL(deviceAdded(OMbusInfo*,OMdeviceInfo*)), _uData, SLOT(deviceAdded(OMbusInfo*,OMdeviceInfo*)), Qt::QueuedConnection);



}

MainWindow::~MainWindow()
{
    delete ui;
    delete _slimWindow;
    delete _filmWindow;
    delete _parser;
    delete _net;
    delete _netModel;
    delete _uData;
}

void MainWindow::on_actionAdd_Bus_triggered() {
    addNetDialog addNet(_net, this);
    addNet.exec();
}

void MainWindow::on_actionAdd_Device_triggered() {
    addDeviceDialog addDev(_net, this);
    addDev.exec();
}


void MainWindow::on_actionOpen_File_triggered() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Script"), "", tr("Slim Scripts (*.slim)"));
    qDebug() << "Got File: " << fileName;
}

void MainWindow::on_actionManage_Network_triggered() {
    networkManager netMan(_netModel, this);
    netMan.exec();
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
