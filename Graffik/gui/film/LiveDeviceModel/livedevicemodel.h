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
    explicit LiveDeviceModel(OMNetwork* c_net, QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;


private slots:
    void _deviceAdded(QString, unsigned short);

private:
    OMNetwork* m_net;
    QList< QHash<QString, unsigned short> > m_cacheDevs;
};

#endif // LIVEDEVICEMODEL_H
