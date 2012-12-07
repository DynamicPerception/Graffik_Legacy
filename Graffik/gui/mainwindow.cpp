/*

  Graffik Motion Control Application

  Copyright (c) 2011-2012 Dynamic Perception

 This file is part of Graffik.

    Graffik is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Graffik is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Graffik.  If not, see <http://www.gnu.org/licenses/>.

    */

#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <QDebug>
#include <QFileDialog>

#include "Slim/SlimFileHandler/slimfilehandler.h"
#include "docs/helpwindow.h"
#include "core/DeviceScan/devicescanner.h"


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    _net = new OMNetwork();
    _axisOpts = new AxisOptions(this);
    _globalOpts = new GlobalOptions(this);
    _netModel = new NetworkModel(_net, this);
    _parser = new SlimCommandParser(_net);
    _cmdHist = new CommandHistoryModel(_net, this);
    _filmWindow = new FilmWindow(_net, _axisOpts, _globalOpts, this);
    _slimWindow =  new SlimWindow(_net, _cmdHist, _parser, ui->tabs);
    _uData = new UserData(this);

    ui->tabs->addWidget(_filmWindow);
    ui->tabs->addWidget(_slimWindow);

    ui->screenSelCombo->addItem(MW_STR_FILM);
    ui->screenSelCombo->addItem(MW_STR_SCRIPT);

    ui->centralwidget->layout()->setMargin(0);
    ui->centralwidget->layout()->setSpacing(0);
    ui->centralwidget->layout()->setContentsMargins(0, 0, 0, 0);


    ui->gridLayout->setContentsMargins(0, 0, 0, 0);
    ui->gridLayout->setMargin(0);

    ui->gridLayout_2->setMargin(0);
    ui->gridLayout_2->setContentsMargins(0, 0, 0, 0);

    // ui->tabs->setContentsMargins(0,0,0,0);

    qRegisterMetaType<GlobalOptions*>("GlobalOptions");

        // make sure that the slim parser can update its alias and registration when a new device is added/deleted
        // (must be done before recoverBuses)
    connect(_net, SIGNAL(deviceAdded(OMbusInfo*,OMdeviceInfo*)), _slimWindow, SLOT(registerNewDevice(OMbusInfo*,OMdeviceInfo*)));
    connect(_net, SIGNAL(deviceDeleted(OMbusInfo*,unsigned short)), _slimWindow, SLOT(removeDevice(OMbusInfo*,unsigned short)));

        // create new options for devices as they are added/deleted
    connect(_net, SIGNAL(deviceAdded(OMbusInfo*,OMdeviceInfo*)), _axisOpts, SLOT(deviceAdded(OMbusInfo*,OMdeviceInfo*)));
    connect(_net, SIGNAL(deviceDeleted(OMbusInfo*,unsigned short)), _axisOpts, SLOT(deviceDeleted(OMbusInfo*,unsigned short)));

        // recover stored bus data back to network manager
        // Do this BEFORE connecting up the signals and slots in uData to
        // avoid wasting time writing back the same data to the
        // ini file.  This recovers all buses and devices
    _uData->recoverBuses(_net);

        // always recover axis options after recovering buses (so devices will have been created already)
        // doing this in the reverse order will result in all devices getting created and then their options
        // overwritten with the default values
    _uData->recoverAxisOptions(_axisOpts);

        // recover global options
    _uData->recoverGlobalOptions(_globalOpts);

        // connect network manager to userdata handlers (to save input information automatically)
    connect(_net, SIGNAL(busAdded(OMbusInfo*)), _uData, SLOT(busAdded(OMbusInfo*)));
    connect(_net, SIGNAL(busDeleted(QString,QString)), _uData, SLOT(busDeleted(QString,QString)));
    connect(_net, SIGNAL(deviceAdded(OMbusInfo*,OMdeviceInfo*)), _uData, SLOT(deviceAdded(OMbusInfo*,OMdeviceInfo*)));
    connect(_net, SIGNAL(deviceDeleted(OMbusInfo*,unsigned short)), _uData, SLOT(deviceDeleted(OMbusInfo*,unsigned short)));

        // again, we connect the userdata to the OMAxisFilmOptions object -after- recoverBuses to prevent the loading
        // of stored axis options to trigger a re-write back to the ini file.  We do not use a queued connection here.
    connect(_axisOpts, SIGNAL(deviceOptionsChanged(OMaxisOptions*,unsigned short)), _uData, SLOT(deviceOptionsChanged(OMaxisOptions*,unsigned short)));
    connect(_axisOpts, SIGNAL(deviceOptionsRemoved(unsigned short)), _uData, SLOT(deviceOptionsRemoved(unsigned short)));

    connect(_globalOpts, SIGNAL(optionsChanged()), this, SLOT(globalOptionsChanged()));
    connect(this, SIGNAL(globalOptionsChanged(GlobalOptions*)), _uData, SLOT(globalOptionsChanged(GlobalOptions*)));

    // simulate pressing the film button to default to film screen
    //on_filmButton_clicked();

    ui->screenSelCombo->setCurrentIndex(0);
    ui->tabs->setCurrentWidget(_filmWindow);

}

MainWindow::~MainWindow() {

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
    delete _globalOpts;
    delete ui;

}


void MainWindow::globalOptionsChanged() {
    emit globalOptionsChanged(_globalOpts);
}


 // TODO: Fix leak
void MainWindow::on_actionHelp_Contents_triggered() {
    qDebug() << "MW: Help Index triggered";
    HelpWindow* help = new HelpWindow(this);
    help->show();
}


void MainWindow::on_actionSettings_triggered() {
    qDebug() << "MW: Settings Triggered";
    GlobalOptionsDialog options(_globalOpts);
    options.exec();
}

void MainWindow::on_actionAbout_Graffik_triggered() {
    qDebug() << "MW: About Triggered";
    AboutDialog about;
    about.exec();
}

/*void MainWindow::on_filmButton_clicked() {
    ui->tabs->setCurrentWidget(_filmWindow);
  //  ui->filmButton->setDown(true);
  //  ui->scriptButton->setDown(false);
}

void MainWindow::on_scriptButton_clicked() {
    ui->tabs->setCurrentWidget(_slimWindow);
  //  ui->filmButton->setDown(false);
  //  ui->scriptButton->setDown(true);
}*/


void MainWindow::on_screenSelCombo_currentIndexChanged(const QString p_str) {
    if( p_str == MW_STR_SCRIPT )
        ui->tabs->setCurrentWidget(_slimWindow);
    else if( p_str == MW_STR_FILM )
        ui->tabs->setCurrentWidget(_filmWindow);
}

void MainWindow::on_netButton_clicked() {
    NetBaseDialog net(_net, _netModel, _axisOpts);
    net.exec();
}

void MainWindow::on_optButton_clicked() {
    on_actionSettings_triggered();
}


