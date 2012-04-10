#include "livedevicemodel.h"

#include <QList>
#include <QString>
#include <QDebug>

LiveDeviceModel::LiveDeviceModel(OMNetwork* c_net, QObject *parent) : QAbstractTableModel(parent) {
    m_net = c_net;
        // listen to signal from network model
    QObject::connect(m_net, SIGNAL(deviceAdded(OMdeviceInfo*)), this, SLOT(_deviceAdded(OMdeviceInfo*)), Qt::QueuedConnection);
}


/* Gets notification when a new device is added to network */
void LiveDeviceModel::_deviceAdded(OMdeviceInfo *p_dev) {
    beginInsertRows(QModelIndex(), this->rowCount(), this->rowCount());
    m_cacheDevs.append(p_dev);
    qDebug() << "LDM Got new dev " << p_dev->name;
    endInsertRows();
}

void LiveDeviceModel::deviceClicked(const QModelIndex &p_item) {
    if( p_item.row() > rowCount() || p_item.column() > 0 )
        return;

    OMdeviceInfo* myDev = m_cacheDevs.at(p_item.row());
    emit deviceSelected(myDev->device->address());
}

int LiveDeviceModel::rowCount(const QModelIndex & parent) const {

    return m_cacheDevs.count();
}

int LiveDeviceModel::columnCount(const QModelIndex &parent) const {
    return 1; // only one column
}

QVariant LiveDeviceModel::data(const QModelIndex &index, int role) const {

  //  qDebug() << "LDM Request for row" << index.row() << " Column " << index.column();

    if( index.row() > rowCount() || index.column() > 0 || role != Qt::DisplayRole )
        return QVariant();

    OMdeviceInfo* myDev = m_cacheDevs.at(index.row());

    if( index.column() == 0 ) {
    //    qDebug() << "LDM Dev: " << myDev->name;
        return QVariant(myDev->name);
    }

}
