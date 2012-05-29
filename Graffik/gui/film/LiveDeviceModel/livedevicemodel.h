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
    explicit LiveDeviceModel(QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;


private slots:

public slots:
    void deviceClicked(const QModelIndex & p_item);
    void deviceAdded(OMdeviceInfo* p_dev);

signals:
    void deviceSelected(unsigned short);

private:

    QList<OMdeviceInfo*> m_cacheDevs;
};

#endif // LIVEDEVICEMODEL_H
