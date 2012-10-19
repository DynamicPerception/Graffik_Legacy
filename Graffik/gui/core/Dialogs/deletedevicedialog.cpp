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

#include "deletedevicedialog.h"
#include "ui_deletedevicedialog.h"

#include <QDebug>

DeleteDeviceDialog::DeleteDeviceDialog(OMNetwork *c_net, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeleteDeviceDialog)
{
    ui->setupUi(this);

    m_net = c_net;

    QList<QString> buses = m_net->getBuses();
    QString bus;

    foreach(bus, buses) {

        QList<unsigned short> devs = m_net->getDevices(bus);
        unsigned short addr;

        foreach(addr, devs) {

            QHash<unsigned short, QString>* loc = new QHash<unsigned short, QString>;

            loc->insert(addr, bus);

            m_devList.append(loc);
            ui->devListCombo->addItem(m_net->deviceInfo(bus, addr)->name);
        }
    }

}

DeleteDeviceDialog::~DeleteDeviceDialog()
{
    delete ui;

    for(int i = 0; i < m_devList.count(); i++) {
        delete m_devList.at(i);
    }

}

void DeleteDeviceDialog::on_okButton_clicked() {


    int dev = ui->devListCombo->currentIndex();

        // don't try and delete an invalid device

    if( dev < 0 || dev > m_devList.count() ) {
        QDialog::accept();
        return;
    }

    QHash<unsigned short, QString>* devInfo = m_devList.at(dev);
    unsigned short addr = devInfo->keys()[0];
    QString bus = devInfo->value(addr);

    qDebug() << "DDD: selected device for deletion:" << bus << addr;

    m_net->deleteDevice(bus, addr);

    QDialog::accept();

}

void DeleteDeviceDialog::on_cancelButton_clicked() {
    QDialog::reject();
}
