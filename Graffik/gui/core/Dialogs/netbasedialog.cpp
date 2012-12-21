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

    on_manageButton_clicked();
}

NetBaseDialog::~NetBaseDialog()
{
    delete ui;

    if( m_scan != 0)
        delete m_scan;

    delete m_frameLayout;
    delete m_netMan;
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

    if( m_scan != 0)
        delete m_scan;

    m_scan = new DeviceScanner(m_net);
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

    if( m_scan != 0)
        delete m_scan;

    m_scan = new DeviceScanner(m_net, 2, true);
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


