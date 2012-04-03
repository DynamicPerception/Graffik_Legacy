#ifndef SPEEDCONTROLPROXY_H
#define SPEEDCONTROLPROXY_H

#include <QObject>
#include <QHash>

#include "MoCoBus/omnetwork.h"

/**

  Provides a proxy class for managing near real-time speed commands to nanoMoCo axes.

  Used within the Film portion of the GUI to proxy changes in the speed dial to the
  currently selected axis.

  @author
  C. A. Church
  */

class SpeedControlProxy : public QObject
{
    Q_OBJECT
public:
    SpeedControlProxy(OMNetwork* c_net, QObject *parent = 0);
    
signals:

public slots:
//    void speedPosChange(int p_value);
    void deviceChange(unsigned short p_addr);

private slots:
    void _deviceAdded(OMdeviceInfo* p_dev);

private:
    OMNetwork* m_net;
    QHash<unsigned short, OMdeviceInfo*> m_devList;
    OMDevice* m_curDev;
};

#endif // SPEEDCONTROLPROXY_H
