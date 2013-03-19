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



MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {

        // handle QGradient metatype registration for our property
    qRegisterMetaType<QGradient>("QGradient");


    ui->setupUi(this);
    m_net = new OMNetwork();
    m_axisOpts = new AxisOptions(this);
    m_globalOpts = new GlobalOptions(this);
    m_netModel = new NetworkModel(m_net, this);
    m_parser = new SlimCommandParser(m_net);
    m_cmdHist = new CommandHistoryModel(m_net, this);
    m_filmWindow = new FilmWindow(m_net, m_axisOpts, m_globalOpts, this);
    m_slimWindow =  new SlimWindow(m_net, m_cmdHist, m_parser, ui->tabs);
    m_uData = new UserData(this);



    ui->tabs->addWidget(m_filmWindow);
    ui->tabs->addWidget(m_slimWindow);

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
    connect(m_net, SIGNAL(deviceAdded(OMbusInfo*,OMdeviceInfo*)), m_slimWindow, SLOT(registerNewDevice(OMbusInfo*,OMdeviceInfo*)));
    connect(m_net, SIGNAL(deviceDeleted(OMbusInfo*,unsigned short)), m_slimWindow, SLOT(removeDevice(OMbusInfo*,unsigned short)));

        // create new options for devices as they are added/deleted
    connect(m_net, SIGNAL(deviceAdded(OMbusInfo*,OMdeviceInfo*)), m_axisOpts, SLOT(deviceAdded(OMbusInfo*,OMdeviceInfo*)));
    connect(m_net, SIGNAL(deviceDeleted(OMbusInfo*,unsigned short)), m_axisOpts, SLOT(deviceDeleted(OMbusInfo*,unsigned short)));

        // recover stored bus data back to network manager
        // Do this BEFORE connecting up the signals and slots in uData to
        // avoid wasting time writing back the same data to the
        // ini file.  This recovers all buses and devices
    m_uData->recoverBuses(m_net);

        // always recover axis options after recovering buses (so devices will have been created already)
        // doing this in the reverse order will result in all devices getting created and then their options
        // overwritten with the default values
    m_uData->recoverAxisOptions(m_axisOpts);

        // recover global options
    m_uData->recoverGlobalOptions(m_globalOpts);

        // connect network manager to userdata handlers (to save input information automatically)
    connect(m_net, SIGNAL(busAdded(OMbusInfo*)), m_uData, SLOT(busAdded(OMbusInfo*)));
    connect(m_net, SIGNAL(busDeleted(QString,QString)), m_uData, SLOT(busDeleted(QString,QString)));
    connect(m_net, SIGNAL(deviceAdded(OMbusInfo*,OMdeviceInfo*)), m_uData, SLOT(deviceAdded(OMbusInfo*,OMdeviceInfo*)));
    connect(m_net, SIGNAL(deviceDeleted(OMbusInfo*,unsigned short)), m_uData, SLOT(deviceDeleted(OMbusInfo*,unsigned short)));

        // again, we connect the userdata to the OMAxisFilmOptions object -after- recoverBuses to prevent the loading
        // of stored axis options to trigger a re-write back to the ini file.  We do not use a queued connection here.
    connect(m_axisOpts, SIGNAL(deviceOptionsChanged(OMaxisOptions*,unsigned short)), m_uData, SLOT(deviceOptionsChanged(OMaxisOptions*,unsigned short)));
    connect(m_axisOpts, SIGNAL(deviceOptionsRemoved(unsigned short)), m_uData, SLOT(deviceOptionsRemoved(unsigned short)));

    connect(m_globalOpts, SIGNAL(optionsChanged()), this, SLOT(_globalOptionsChanged()));
    connect(this, SIGNAL(globalOptionsChanged(GlobalOptions*)), m_uData, SLOT(globalOptionsChanged(GlobalOptions*)));

    // default to film screen

    ui->screenSelCombo->setCurrentIndex(0);
    ui->tabs->setCurrentWidget(m_filmWindow);
    m_curScreen = FILM_SCREEN;

        // themeing, watch for changes

    Themer* theme = &Singleton<Themer>::Instance();
    connect(theme, SIGNAL(themeChanged()), this, SLOT(_themeChanged()));

    setStyleSheet(theme->getThemeCSS("main"));

        // if the application has not yet been configured, prompt a welcome screen
    if( ! m_uData->exists() ) {
        WelcomeDialog welcome(this);
        connect(&welcome, SIGNAL(addBus()), this, SLOT(_welcomeAddBus()));
        welcome.exec();
        disconnect(&welcome, SIGNAL(addBus()), this, SLOT(_welcomeAddBus()));
    }

    m_filmWindow->postInitialize();

}

MainWindow::~MainWindow() {

        // ALWAYS delete userdata first to prevent it catching
        // signals as other classes are destroyed

    delete m_uData;
    delete m_filmWindow;
    delete m_slimWindow;
    delete m_parser;
    delete m_cmdHist;
    delete m_netModel;
    delete m_axisOpts;
    delete m_net;
    delete m_globalOpts;
    delete ui;

}


void MainWindow::_globalOptionsChanged() {
    emit globalOptionsChanged(m_globalOpts);
}

void MainWindow::_optionsCleared() {
    ConfirmDialog dia("WARNING: Clearing All Settings Cannot Be Undone, Continue?", this);
    int res = dia.exec();
    if( res == QDialog::Accepted ) {
        m_uData->clear();
    }
}

void MainWindow::_themeChanged() {
    setStyleSheet(SingleThemer::getStyleSheet("main"));
    Themer::rePolish(this);
}

void MainWindow::_welcomeAddBus() {

    AddNetDialog addnet(m_net, this);
    addnet.exec();
}

 // TODO: Fix leak
void MainWindow::on_actionHelp_Contents_triggered() {
    qDebug() << "MW: Help Index triggered";
    HelpWindow* help = new HelpWindow(this);
    help->show();
}


void MainWindow::on_actionSettings_triggered() {
    qDebug() << "MW: Settings Triggered";
    GlobalOptionsDialog options(m_globalOpts);

    connect(&options, SIGNAL(optionsCleared()), this, SLOT(_optionsCleared()));

    options.exec();

    disconnect(&options, SIGNAL(optionsCleared()), this, SLOT(_optionsCleared()));
}

void MainWindow::on_actionAbout_Graffik_triggered() {
    qDebug() << "MW: About Triggered";
    AboutDialog about;
    about.exec();
}



void MainWindow::on_screenSelCombo_currentIndexChanged(const QString p_str) {
    if( p_str == MW_STR_SCRIPT ) {
        ui->tabs->setCurrentWidget(m_slimWindow);
        m_curScreen = SLIM_SCREEN;
    }
    else if( p_str == MW_STR_FILM ) {
        ui->tabs->setCurrentWidget(m_filmWindow);
        m_curScreen = FILM_SCREEN;
    }

}

void MainWindow::on_netButton_clicked() {
    NetBaseDialog net(m_net, m_netModel, m_axisOpts);
    net.exec();
}

void MainWindow::on_optButton_clicked() {
    on_actionSettings_triggered();
}

void MainWindow::on_globalLoadButton_clicked() {
    if( m_curScreen == FILM_SCREEN )
        m_filmWindow->load();
    else if( m_curScreen == SLIM_SCREEN)
        m_slimWindow->load();
}

void MainWindow::on_globalSaveButton_clicked() {
    if( m_curScreen == FILM_SCREEN )
        m_filmWindow->save();
    else if( m_curScreen == SLIM_SCREEN )
        m_slimWindow->save();
}
