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
 //   _filmWindow = new filmWindow(this);

    ui->tabs->addTab(_slimWindow, "Slim");
//    ui->tabs->addTab(_filmWindow, "Film");

}

MainWindow::~MainWindow()
{
    delete ui;
    delete _slimWindow;
 //   delete _filmWindow;
    delete _parser;
    delete _net;
    delete _netModel;
}

void MainWindow::on_actionAdd_Bus_triggered() {
    addNetDialog addNet(_net, this);
    addNet.exec();
}

void MainWindow::on_actionAdd_Device_triggered() {
    addDeviceDialog addDev(_net, _parser, this);
    addDev.exec();
}


void MainWindow::on_actionOpen_File_triggered() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Script"), "", tr("Slim Scripts (*.slim)"));
    qDebug() << "Got File: " << fileName;
}

void MainWindow::on_actionManage_Network_triggered() {
    networkManager netMan(_netModel, _parser, this);
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
