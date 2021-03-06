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

#include <QIcon>

#include "networkmodel.h"

NetworkModel::NetworkModel(OMNetwork* c_net, QObject *parent) : QStandardItemModel(parent) {
    m_net = c_net;
    _setHeaders();

    m_icon = new QPixmap;

        // listen in for buses or devices being added to the network, so that we
        // may automatically add them to the model

    QObject::connect(m_net, SIGNAL(busAdded(QString)), this, SLOT(addBus(QString)));
    QObject::connect(m_net, SIGNAL(busDeleted(QString,QString)), this, SLOT(busDeleted(QString,QString)));

    QObject::connect(m_net, SIGNAL(deviceAdded(QString, unsigned short)), this, SLOT(addDevice(QString, unsigned short)));
    QObject::connect(m_net, SIGNAL(deviceDeleted(QString,unsigned short)), this, SLOT(deviceDeleted(QString,unsigned short)));


}

NetworkModel::~NetworkModel() {

}

void NetworkModel::_setHeaders() {
    QStringList headerLabels;
    headerLabels << "Name";
    headerLabels << "Address";
    headerLabels << "Type";
    headerLabels << "Configure";

    this->setHorizontalHeaderLabels(headerLabels);

}

OMNetwork* NetworkModel::net() {
    return m_net;
}

void NetworkModel::addBus(QString p_port) {

    qDebug() << "NWM: Adding Bus" << p_port;
    QStandardItem* parent = invisibleRootItem();

    OMbusInfo* bus = m_net->busInfo(p_port);
    QStandardItem* newItem = new QStandardItem(bus->name);

        // prevent editing
    newItem->setFlags(newItem->flags() & ~Qt::ItemIsEditable);
        // four child columns
    newItem->setColumnCount(4);

    parent->appendRow(newItem);

}

void NetworkModel::addDevice(QString p_port, unsigned short p_addr) {

    QString name = m_net->busInfo(p_port)->name;
    QList<QStandardItem*> parentList = findItems(name);

    if( parentList.length() < 1 || parentList.length() > 1 ) {
        qDebug() << "NWM: Very odd, too few or too many entries found for " << p_port;
        return;
    }

    OMdeviceInfo* dev = m_net->deviceInfo(p_port, p_addr);

    QList<QStandardItem*> display;
    QString addr;

    addr.setNum(dev->device->address());

    display.append(new QStandardItem(dev->name));
    display.append(new QStandardItem(addr));
    display.append(new QStandardItem(dev->type));
    display.append(new QStandardItem(QIcon(*m_icon), ""));


        // prevent editing
    display[0]->setFlags(display[0]->flags() & ~Qt::ItemIsEditable);
    display[1]->setFlags(display[1]->flags() & ~Qt::ItemIsEditable);
    display[2]->setFlags(display[2]->flags() & ~Qt::ItemIsEditable);
    display[3]->setFlags(display[3]->flags() & ~Qt::ItemIsEditable);

        // tooltip for configure icon
        // TODO: Correct issue preventing tooltip from displaying
    display[3]->setFlags(display[3]->flags() | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    display[3]->setToolTip(NETMODEL_STR_CONF);

    parentList[0]->appendRow(display);

}

void NetworkModel::deviceDeleted(QString p_port, unsigned short p_addr) {

    QString busName = m_net->busInfo(p_port)->name;
    QList<QStandardItem*> parentList = findItems(busName);

    if( parentList.length() < 1 )
        return;


    int delRow = -1;

    for(int i = 0; i < parentList[0]->rowCount(); i++) {
        QStandardItem* addr = parentList[0]->child(i,1);
        qDebug() << "NWM: Iterator " << i << " of " << parentList[0]->rowCount();
        unsigned short matchAddr = addr->data(Qt::DisplayRole).toUInt();
        if( matchAddr == p_addr ) {
            qDebug() << "NWM: Addresses match!";
            delRow = i;
            break;
        }
    }


    if( delRow != -1 ) {
        qDebug() << "NWM: Got row" << delRow;
        beginRemoveRows(parentList[0]->index(), delRow, delRow);
        parentList[0]->removeRow(delRow);
        endRemoveRows();
        qDebug() << "NWM: Removed Device" << p_addr;
    }


}

void NetworkModel::busDeleted(QString p_port, QString p_name) {
    qDebug() << "NWM: Got Remove Bus" << p_port << p_name;
    QList<QStandardItem*> parentList = findItems(p_name);
    QStandardItem* parent = invisibleRootItem();

    if( parentList.length() < 1 )
        return;

    QModelIndex idx = indexFromItem(parentList[0]);

    beginRemoveRows(parent->index(), idx.row(), idx.row());
    parent->removeRow(idx.row());
    endRemoveRows();

    qDebug() << "NWM: Removed Bus" << p_port << p_name;

}

QPixmap NetworkModel::editIcon() {
    return *m_icon;
}

void NetworkModel::setEditIcon(QPixmap p_img) {
    qDebug() << "NWM: SetEditIcon";

    *m_icon = p_img;

    // We must replace all existing configure icons from the model

    int rows = rowCount();

    if( rows < 1)
        return;

    for(int i = 0; i < rows; i++ ) {

        QStandardItem* rowItem = item(i);

        for( int f = 0; f < rowItem->rowCount(); f++ ) {
            qDebug() << "NWM: Set icon for" << i << f;
           // QStandardItem* curItem = rowItem->child(f, 3);
           // delete curItem;
            rowItem->setChild(f, 3, new QStandardItem(QIcon(*m_icon), ""));
        }
    }

}
