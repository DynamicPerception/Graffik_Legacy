#ifndef HOMEMONITOR_H
#define HOMEMONITOR_H

#include <QObject>
#include <QList>
#include <QTimer>

#include "MoCoBus/omnetwork.h"
#include "Devices/nanoMoCo/omaxis.h"


#define HMM_PERIOD  500
#define HMM_SLOP    10

/** Home Monitor Class

  The Home Monitor Class monitors a specified set of axes, reporting
  the allAtHome() signal when all axes have arrived at the home position.

  This class uses a QTimer to periodically check the position of each specified
  axis by querying it for its distance from home.

  If any error occurs, the error() signal will be emitted.

  @author C. A. Church
  */

class HomeMonitor : public QObject
{
    Q_OBJECT
public:
    explicit HomeMonitor(OMNetwork* c_net, QObject *parent = 0);
    ~HomeMonitor();


        /** Monitor these Axes */
    void checkAxes(QList<OMAxis*> p_axes);

        /** Start the QTimer */
    void start();
        /** Stop the QTimer */
    void stop();

signals:
        /** All Monitored Axes are at Home */
    void allAtHome();
        /** An Error Occurred */
    void error(QString p_err);

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
