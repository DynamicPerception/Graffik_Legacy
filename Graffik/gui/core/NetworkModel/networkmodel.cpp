#include <QDebug>

#include <QIcon>

#include "networkmodel.h"

networkModel::networkModel(OMNetwork* c_net, QObject *parent) :
    QStandardItemModel(parent)
{
    m_net = c_net;

    QStringList headerLabels;
    headerLabels << "Name";
    headerLabels << "Address";
    headerLabels << "Type";
    headerLabels << "Actions";

    this->setHorizontalHeaderLabels(headerLabels);

        // listen in for buses or devices being added to the network, so that we
        // may automatically add them to the model

    QObject::connect(m_net, SIGNAL(busAdded(QString)), this, SLOT(addBus(QString)), Qt::QueuedConnection);
    QObject::connect(m_net, SIGNAL(deviceAdded(QString, unsigned short)), this, SLOT(addDevice(QString, unsigned short)), Qt::QueuedConnection);
}

networkModel::~networkModel() {

}

OMNetwork* networkModel::net() {
    return m_net;
}

void networkModel::addBus(QString p_port) {
    QStandardItem* parent = invisibleRootItem();

    OMbusInfo* bus = m_net->busInfo(p_port);
    QStandardItem* newItem = new QStandardItem(bus->name);

        // prevent editing
    newItem->setFlags(newItem->flags() & ~Qt::ItemIsEditable);
        // four child columns
    newItem->setColumnCount(4);

    parent->appendRow(newItem);

}

void networkModel::addDevice(QString p_port, unsigned short p_addr) {

    QString name = m_net->busInfo(p_port)->name;
    QList<QStandardItem*> parentList = findItems(name);

    if( parentList.length() < 1 || parentList.length() > 1 ) {
        qDebug() << "Very odd, too few or too many entries found for " << p_port;
        return;
    }

    OMdeviceInfo* dev = m_net->deviceInfo(p_port, p_addr);

    QList<QStandardItem*> display;
    QString addr;

    addr.setNum(dev->device->address());

    display.append(new QStandardItem(dev->name));
    display.append(new QStandardItem(addr));
    display.append(new QStandardItem(dev->type));
    display.append(new QStandardItem(QIcon(":icons/img/configure.png"), ""));


        // prevent editing
    display[0]->setFlags(display[0]->flags() & ~Qt::ItemIsEditable);
    display[1]->setFlags(display[1]->flags() & ~Qt::ItemIsEditable);
    display[2]->setFlags(display[2]->flags() & ~Qt::ItemIsEditable);
    display[3]->setFlags(display[2]->flags() & ~Qt::ItemIsEditable);

    parentList[0]->appendRow(display);

}

void networkModel::deviceDeleted(QString p_port, unsigned short p_addr) {

    QString busName = m_net->busInfo(p_port)->name;
    QList<QStandardItem*> parentList = findItems(busName);

    if( parentList.length() < 1 )
        return;

    int delRow = -1;

    for(int i = 0; i < parentList[0]->rowCount(); i++) {
        QStandardItem* addr = parentList[0]->child(i,1);
        if( addr->data().toUInt() == p_addr ) {
            delRow = i;
            break;
        }
    }

    if( delRow != -1 ) {
        parentList[0]->removeRow(delRow);
    }

    qDebug() << "NWM: Removed Device" << p_addr;
}
