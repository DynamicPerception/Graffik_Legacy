#ifndef USERDATA_H
#define USERDATA_H

#include <QSettings>
#include <QString>
#include <QHash>
#include <QVariant>

#include "libs/OpenMoCo/MoCoBus/omnetwork.h"

class UserData
{
public:
    UserData();
    ~UserData();

public slots:
    void busAdded(OMbusInfo* p_bus);
    void deviceAdded(OMdeviceInfo* p_dev);

private:
    QSettings* m_qset;
    QHash<QString, QVariant>* m_buses;
    QHash<QString, QVariant>* m_devices;

};

#endif // USERDATA_H
