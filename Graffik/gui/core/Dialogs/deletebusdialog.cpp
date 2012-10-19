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

#include "deletebusdialog.h"
#include "ui_deletebusdialog.h"

#include <QDebug>


DeleteBusDialog::DeleteBusDialog(OMNetwork *c_net, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeleteBusDialog)
{
    ui->setupUi(this);
    m_net = c_net;

    QList<QString> buses = m_net->getBuses();
    QString bus;

    foreach(bus, buses) {
        ui->busListCombo->addItem(m_net->busInfo(bus)->name);
        m_buses.append(bus);
    }


}

DeleteBusDialog::~DeleteBusDialog()
{
    delete ui;
}

void DeleteBusDialog::on_okButton_clicked() {

    int busPos = ui->busListCombo->currentIndex();

    // don't try and delete an invalid bus

    if( busPos < 0 || busPos > m_buses.count() ) {
       QDialog::accept();
       return;
    }

    QString bus = m_buses.at(busPos);

    qDebug() << "DBD: Sending Delete for bus" << bus;

    try {
        m_net->deleteBus(bus);
    }
    catch (int e) {
        qDebug() << "DBD: Received exception from Network:" << e;
    }

    qDebug() << "DBD: Sent Delete for bus" << bus;

    QDialog::accept();
    //QDialog::done(1);

}


void DeleteBusDialog::on_cancelButton_clicked() {
    QDialog::reject();
}
