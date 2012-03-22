#include "livedevicemodel.h"

#include <QList>
#include <QString>
#include <QDebug>

LiveDeviceModel::LiveDeviceModel(OMNetwork* c_net, QObject *parent) : QAbstractTableModel(parent) {
    m_net = c_net;
        // listen to signal from network model
    QObject::connect(m_net, SIGNAL(deviceAdded(QString,unsigned short)), this, SLOT(_deviceAdded(QString,unsigned short)), Qt::QueuedConnection);
}


/* Gets notification when a new device is added to network */
void LiveDeviceModel::_deviceAdded(QString c_str, unsigned short c_addr) {
    beginInsertRows(QModelIndex(), this->rowCount(), this->rowCount());
    QHash<QString, unsigned short> thsDev;
    thsDev.insert(c_str, c_addr);
    m_cacheDevs.append(thsDev);
    qDebug() << "LDM Got new dev " << c_addr;
    endInsertRows();
}

int LiveDeviceModel::rowCount(const QModelIndex & parent) const {

    return m_cacheDevs.count();
}

int LiveDeviceModel::columnCount(const QModelIndex &parent) const {
    return 1; // only one column
}

QVariant LiveDeviceModel::data(const QModelIndex &index, int role) const {

    qDebug() << "LDM Request for row" << index.row() << " Column " << index.column();

    if( index.row() > rowCount() || index.column() > 0 || role != Qt::DisplayRole )
        return QVariant();

    QHash<QString, unsigned short> myRow = m_cacheDevs.at(index.row());

    if( index.column() == 0 ) {
        QString bus = myRow.keys().at(0);
        unsigned short addr = myRow.value(bus);
        QString name = m_net->deviceInfo(bus, addr)->name;
        qDebug() << "Dev: " << bus << addr << name;
        return QVariant(name);
    }

}
