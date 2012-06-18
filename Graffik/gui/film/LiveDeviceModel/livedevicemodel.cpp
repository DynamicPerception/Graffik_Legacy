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
    m_devCacheLookup.insert(p_dev->device->address(), m_cacheDevs.size() - 1);
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

    return QVariant(myDev->name);
}

int LiveDeviceModel::find(unsigned short p_addr) {
    if( ! m_devCacheLookup.contains(p_addr) )
        return -1;

    return m_devCacheLookup.value(p_addr);
}

void LiveDeviceModel::deviceDeleted(QString p_bus, unsigned short p_addr) {


    int pos = find(p_addr);

    qDebug() << "LDM: Removing device" << p_addr << pos;

    if( pos == -1 )
        return;

    beginRemoveRows(QModelIndex(), pos, pos);
    m_cacheDevs.removeAt(pos);
    endRemoveRows();

    m_devCacheLookup.remove(p_addr);

        // update index, move all rows above this one down one row
    foreach(unsigned short key, m_devCacheLookup.keys()) {
        if( m_devCacheLookup.value(key) > pos)
            m_devCacheLookup[key]--;
    }
}
