#ifndef NETWORKMODEL_H
#define NETWORKMODEL_H

#include <QStandardItemModel>

#include "MoCoBus/omnetwork.h"

  // which column holds the configure icon?
#define NETMODEL_CONFIG_COL 3
#define NETMODEL_ADDR_COL 1

class NetworkModel : public QStandardItemModel
{
    Q_OBJECT
public:
    NetworkModel(OMNetwork* c_net, QObject *parent = 0);
    ~NetworkModel();

    OMNetwork* net();

signals:
    
public slots:

    void addBus(QString);
    void busDeleted(QString, QString);
    void addDevice(QString, unsigned short);
    void deviceDeleted(QString, unsigned short);

private:
    OMNetwork* m_net;

    void _setHeaders();
};

#endif // NETWORKMODEL_H
