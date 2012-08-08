#ifndef HOMEMONITOR_H
#define HOMEMONITOR_H

#include <QObject>
#include <QList>
#include <QTimer>

#include "MoCoBus/omnetwork.h"
#include "Devices/nanoMoCo/omaxis.h"

#define HMM_PERIOD  500
#define HMM_SLOP    10

class HomeMonitor : public QObject
{
    Q_OBJECT
public:
    explicit HomeMonitor(OMNetwork* c_net, QObject *parent = 0);
    ~HomeMonitor();


    void checkAxes(QList<OMAxis*> p_axes);

    void start();
    void stop();

signals:
    void allAtHome();

public slots:

private slots:
    void _timerFire();
    void _cmdReceived(int p_id, OMCommandBuffer* p_cmd);

private:

    OMNetwork* m_net;
    QTimer* m_timer;
    QList<OMAxis*> m_axes;
    QHash<int, OMAxis*> m_cmds;

    bool m_started;

};

#endif // HOMEMONITOR_H
