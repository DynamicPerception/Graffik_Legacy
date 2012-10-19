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

#include <QDebug>

#include "networkmanager.h"
#include "ui_networkmanager.h"

#include "core/Options/axisoptionsdialog.h"


NetworkManager::NetworkManager(NetworkModel *c_net, AxisOptions* c_opts, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NetworkManager)
{
    ui->setupUi(this);

    _netMod = c_net;
    _opts = c_opts;

    ui->netTreeView->setModel(c_net);

    ui->netTreeView->expandAll();

    QObject::connect(ui->netTreeView, SIGNAL(clicked(QModelIndex)), this, SLOT(onRowClick(QModelIndex)), Qt::QueuedConnection);

}

NetworkManager::~NetworkManager()
{
    delete ui;
}

void NetworkManager::on_addBusButton_clicked() {
    AddNetDialog addNet(_netMod->net(), this);
    addNet.exec();
}

void NetworkManager::on_addDevButton_clicked() {
    AddDeviceDialog addDev(_netMod->net(), this);
    addDev.exec();
}

void NetworkManager::on_delDevButton_clicked() {
    DeleteDeviceDialog delDev(_netMod->net(), this);
    delDev.exec();
}

void NetworkManager::on_delBusButton_clicked() {
    DeleteBusDialog delBus(_netMod->net(), this);
    delBus.exec();
}

void NetworkManager::onRowClick(QModelIndex p_idx) {
    qDebug() << "NM: Got click on" << p_idx.row() << p_idx.column();

        // if they clicked the config button, retrieve address for item
    if( p_idx.column() == NETMODEL_CONFIG_COL ) {
        qDebug() << "NM: Hit config link";

            // model is nested by bus...
        const QModelIndex parent = p_idx.parent();
        QModelIndex nidx = _netMod->index(p_idx.row(), NETMODEL_ADDR_COL, parent);
            // get item
        const QStandardItem* itm = _netMod->itemFromIndex(nidx);

        unsigned short addr = itm->text().toUShort();
        qDebug() << "NM: Spawn Config for address" << addr;

        AxisOptionsDialog opts(_opts, addr, this);
        opts.exec();
    }
}

