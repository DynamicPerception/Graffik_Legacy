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

#include "netbasedialog.h"
#include "ui_netbasedialog.h"



NetBaseDialog::NetBaseDialog(OMNetwork *c_net, NetworkModel *c_model, AxisOptions *c_opts, QWidget *parent) : QDialog(parent), ui(new Ui::NetBaseDialog) {

    ui->setupUi(this);

            m_net = c_net;
           m_opts = c_opts;
          m_model = c_model;
           m_scan = 0;
      m_curWidget = 0;
    m_frameLayout = new QVBoxLayout();

    m_frameLayout->setSpacing(0);
    m_frameLayout->setContentsMargins(0,0,0,0);
    ui->actionFrame->setLayout(m_frameLayout);

    m_netMan = new NetworkManager(m_model, m_opts, this);

        // prevent the close or minimize system window option from being used

    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint);

        // default to manage devices screen
    on_manageButton_clicked();

    setStyleSheet(SingleThemer::getStyleSheet("net_base"));
}

NetBaseDialog::~NetBaseDialog() {
    delete ui;

    if( m_scan != 0) {
        disconnect(m_scan, SIGNAL(scanDone()), this, SLOT(scanCompleted()));
        delete m_scan;
    }

    delete m_frameLayout;
    delete m_netMan;
}


/** Scan Completed Slot

  Triggering a scan disables the 'Done' button on this dialog.

  This slot re-enables the 'Done' button when the scan is completed.
  */

void NetBaseDialog::scanCompleted() {
    _blockForScan(false);
}

void NetBaseDialog::on_scanButton_clicked() {

    ui->initButton->setDown(false);
    ui->scanButton->setDown(true);
    ui->manageButton->setDown(false);

    if( m_curState == SCAN )
        return;

    // the devicescanner destroys its widget when destroyed,
    // so let's make sure we don't try to access it later in
    // _swapWidget

    if( m_curState == INIT )
        m_curWidget = 0;

    m_curState = SCAN;

    if( m_scan != 0) {
        disconnect(m_scan, SIGNAL(scanDone()), this, SLOT(scanCompleted()));
        delete m_scan;
    }

    m_scan = new DeviceScanner(m_net);

        // monitor signal for when we can re-enable done button
    connect(m_scan, SIGNAL(scanDone()), this, SLOT(scanCompleted()));

    _blockForScan(true);

    m_scan->startScan();

    QWidget* dispWidg = m_scan->getWidget();

    _swapWidget(dispWidg);
}


void NetBaseDialog::on_initButton_clicked() {

    ui->initButton->setDown(true);
    ui->scanButton->setDown(false);
    ui->manageButton->setDown(false);

    if( m_curState == INIT )
        return;

        // the devicescanner destroys its widget when destroyed,
        // so let's make sure we don't try to access it later in
        // _swapWidget

    if( m_curState == SCAN )
        m_curWidget = 0;

    m_curState = INIT;

    if( m_scan != 0) {
        disconnect(m_scan, SIGNAL(scanDone()), this, SLOT(scanCompleted()));
        delete m_scan;
    }

    m_scan = new DeviceScanner(m_net, 2, true);

        // monitor signal for when we can re-enable done button
    connect(m_scan, SIGNAL(scanDone()), this, SLOT(scanCompleted()));

    _blockForScan(true);

    m_scan->startScan();

    QWidget* dispWidg = m_scan->getWidget();

    _swapWidget(dispWidg);
}

void NetBaseDialog::on_manageButton_clicked() {


    ui->initButton->setDown(false);
    ui->scanButton->setDown(false);
    ui->manageButton->setDown(true);

    if( m_curState == MANAGE )
        return;

    m_curState = MANAGE;

    _swapWidget(m_netMan);
}


void NetBaseDialog::on_doneButton_clicked() {

    done(1);
}

 // remove a widget from the display and replace it
void NetBaseDialog::_swapWidget(QWidget *p_widg) {


    if( m_curWidget != 0 ) {
        m_frameLayout->removeWidget(m_curWidget);
        m_curWidget->hide();
    }

    m_frameLayout->addWidget(p_widg);

    p_widg->show();
    m_curWidget = p_widg;
}

 // block/enable UI elements that would interfere with scanning
void NetBaseDialog::_blockForScan(bool p_stat) {

    if( p_stat == true ) {
            // we want to prevent the user from minimizing the window via standard ui elements, so
            // we don't end up with a runaway scan causing problems for the user

        // NOTE: CC: Well, this is a bummer.  Since we're a modal dialog, the dialog will exit
        // when attempting to change window flags after the ctor.  This is because the dialog
        // exits whenever it is hidden (being modal and all).  Thus, we must simply disable
        // closing of the dialog period in the ctor

        // this->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint);
        ui->doneButton->setEnabled(false);
    }
    else {
            // See note above

        //this->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
        //this->show();
        ui->doneButton->setEnabled(true);
    }
}


