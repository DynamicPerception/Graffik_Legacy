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
}

NetBaseDialog::~NetBaseDialog()
{
    delete ui;

    if( m_scan != 0)
        delete m_scan;

}


void NetBaseDialog::on_scanButton_clicked() {
    if( m_scan != 0)
        delete m_scan;

    m_scan = new DeviceScanner(m_net);
}


void NetBaseDialog::on_initButton_clicked() {
    if( m_scan != 0)
        delete m_scan;

    m_scan = new DeviceScanner(m_net, 2);
}

void NetBaseDialog::on_manageButton_clicked() {
    NetworkManager netMan(m_model, m_opts, this);
    netMan.exec();
}
