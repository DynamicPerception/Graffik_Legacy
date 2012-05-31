#include "livedevicemodel.h"

#include <QList>
#include <QString>
#include <QDebug>

LiveDeviceModel::LiveDeviceModel(QObject *parent) : QAbstractTableModel(parent) {


}


/* Gets notification when a new device is added to network */
void LiveDeviceModel::deviceAdded(OMdeviceInfo *p_dev) {
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

int LiveDeviceModel::find(unsigned short p_addr) {

    for(int i = 0; i < m_cacheDevs.size(); ++i) {
        OMdeviceInfo* dev = m_cacheDevs[i];
        if( dev->device->address() == p_addr ) {
            return i;
        }
    }
}
