#ifndef SPEEDCONTROLPROXY_H
#define SPEEDCONTROLPROXY_H

#include <QObject>
#include <QThread>
#include <QHash>
#include <QTimer>

#include "livedevicemodel.h"

#include "MoCoBus/omnetwork.h"
#include "Devices/nanoMoCo/omaxis.h"
#include "core/Options/axisoptions.h"

    // timer period mS - changes speed every period during
    // damping
#define SCP_TIME_PERIOD 100

/**

  Provides a proxy class for managing near real-time speed commands to nanoMoCo axes.

  Used within the Film portion of the GUI to proxy changes in the speed dial to the
  currently selected axis.

  Sends commands directly to axis, does not check to see if command is successfully
  received.

  Provides damping through the use of a continuous fire timer.

  @author
  C. A. Church
  */

class SpeedControlProxy : public QObject
{
    Q_OBJECT
public:
    explicit SpeedControlProxy(AxisOptions *c_opts);
    ~SpeedControlProxy();
    
    void maxSpeed(float p_pct);

    void setDamping(float p_secs);
    void startDampTimer();
    void stopDampTimer();

    void setResolution(unsigned int p_ms);

signals:
    void motorNotReady(unsigned short p_addr);
    void motorChangeAccepted(unsigned short p_addr);

public slots:
    void speedPosChange(int p_value);
    void deviceChange(unsigned short p_addr);
    void deviceAdded(OMdeviceInfo* p_dev);
    void deviceRemoved(QString p_bus, unsigned short p_addr);


private slots:

    void _timerFire();

private:

    QHash<unsigned short, OMdeviceInfo*> m_devList;
    OMAxis* m_curDev;
    OMaxisOptions* m_opts;
    AxisOptions* m_optObj;
    float m_maxpct;
    bool m_curDir;
    float m_curSpd;
    float m_nextSpd;

    unsigned int m_dampPeriods;
    unsigned int m_curPeriod;

    QTimer* m_timer;

    bool m_devSelected;

    void _changeDir(bool p_dir);

};

#endif // SPEEDCONTROLPROXY_H
