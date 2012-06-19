#include "livedevicemodel.h"

#include <QList>
#include <QString>
#include <QDebug>

LiveDeviceModel::LiveDeviceModel(OMNetwork *c_net, QObject *parent) : QAbstractTableModel(parent) {

    m_net = c_net;

    QObject::connect(m_net, SIGNAL(deviceAdded(OMdeviceInfo*)), this, SLOT(deviceAdded(OMdeviceInfo*)));
    QObject::connect(m_net, SIGNAL(deviceDeleted(QString,unsigned short)), this, SLOT(deviceDeleted(QString,unsigned short)));

}


/* Gets notification when a new device is added to network */
void LiveDeviceModel::deviceAdded(OMdeviceInfo *p_dev) {
    beginResetModel();

    m_cacheNet = m_net->getDevices();
    m_devPos = m_cacheNet.keys();

    qDebug() << "LDM: Rebuilt model because device was added:" << p_dev->name;
    endResetModel();
}

void LiveDeviceModel::deviceClicked(const QModelIndex &p_item) {
    if( p_item.row() > rowCount() || p_item.column() > 0 )
        return;

    unsigned short addr = m_devPos.at(p_item.row());
    emit deviceSelected(addr);
}

int LiveDeviceModel::rowCount(const QModelIndex & parent) const {

    return m_cacheNet.count();
}

int LiveDeviceModel::columnCount(const QModelIndex &parent) const {
    return 1; // only one column
}

QVariant LiveDeviceModel::data(const QModelIndex &index, int role) const {

    if( index.row() > rowCount() || index.column() > 0 || role != Qt::DisplayRole )
        return QVariant();

    unsigned short addr = m_devPos.at(index.row());

    return QVariant(m_cacheNet.value(addr)->name);

}

int LiveDeviceModel::find(unsigned short p_addr) {

    return m_devPos.indexOf(p_addr);
}

void LiveDeviceModel::deviceDeleted(QString p_bus, unsigned short p_addr) {


    beginResetModel();

    m_cacheNet = m_net->getDevices();
    m_devPos = m_cacheNet.keys();

    endResetModel();
    qDebug() << "LDM: Rebuilt model because device was removed:" << p_bus << p_addr;
}
