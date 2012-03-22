#include "speedcontrolproxy.h"

#include <QDebug>

SpeedControlProxy::SpeedControlProxy(OMNetwork* c_net, QObject *parent) :
    QObject(parent)
{
    m_net = c_net;
    m_curDev = 0;

    QObject::connect(m_net, SIGNAL(deviceAdded(QString,unsigned short)), this, SLOT(_deviceAdded(QString,unsigned short)), Qt::QueuedConnection);
}


void SpeedControlProxy::_deviceAdded(QString p_bus, unsigned short p_addr) {
    qDebug() << "SCP: Received new device" << p_bus << p_addr;
    OMdeviceInfo* devinfo = m_net->deviceInfo(p_bus, p_addr);
    m_devList.insert(p_addr, devinfo->device);
}
