#ifndef LIVEDEVICEMODEL_H
#define LIVEDEVICEMODEL_H

#include <QAbstractTableModel>
#include <QModelIndex>
#include <QVariant>
#include <QList>

#include "MoCoBus/omnetwork.h"


class LiveDeviceModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    LiveDeviceModel(OMNetwork* c_net, QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;


private slots:
    void _deviceAdded(OMdeviceInfo* p_dev);

public slots:
    void deviceClicked(const QModelIndex & p_item);

signals:
    void deviceSelected(unsigned short);

private:
    OMNetwork* m_net;
    SpeedControlProxy* m_spd;
    QList<OMdeviceInfo*> m_cacheDevs;
};

#endif // LIVEDEVICEMODEL_H
