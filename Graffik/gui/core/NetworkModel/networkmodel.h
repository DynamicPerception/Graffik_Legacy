#ifndef NETWORKMODEL_H
#define NETWORKMODEL_H

#include <QStandardItemModel>

#include "MoCoBus/omnetwork.h"

class networkModel : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit networkModel(OMNetwork* c_net, QObject *parent = 0);
    ~networkModel();

    OMNetwork* net();

signals:
    
public slots:

    void addBus(QString);
    void addDevice(QString, unsigned short);

private:
    OMNetwork* m_net;
};

#endif // NETWORKMODEL_H
