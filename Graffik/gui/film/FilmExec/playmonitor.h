#ifndef PLAYMONITOR_H
#define PLAYMONITOR_H

#include <QObject>
#include <QTimer>
#include <QHash>


#include "MoCoBus/omnetwork.h"
#include "Devices/nanoMoCo/omaxis.h"

#include "core/Options/globaloptions.h"

#include "film/FilmParameters/filmparameters.h"


#define PM_TIMER_PERIOD 250


/** Play State Monitor

  The Play State Manager class provides a mechanism for managing and monitoring the current status of the
  play state of the film.  It is designed to be run in a thread, and uses a timer to regularly check the
  timing from the master node.

  This class, when informed to run, emits a signal once per timing period with the current status of the
  master node and the current execution time in milliseconds.

  Direct interaction with this class is only intended to be done by the FilmExec class.  Any signals emitted
  by this class will be reflected by it.

  If, at any point, the Timing Master Node cannot be reached, or responds with an error, the error() signal will
  be emitted.

  @author C. A. Church
  Copyright &copy 2012 Dynamic Perception LLC

  */

class PlayMonitor : public QObject
{
    Q_OBJECT
public:
    PlayMonitor(OMNetwork* c_net, FilmParameters* c_film, GlobalOptions* c_gopts, QObject *parent = 0);
    ~PlayMonitor();

    void start();
    void stop();
    void master(OMAxis* p_master);
    
signals:

    void playStatus(bool p_stat, unsigned long p_runTime);
    void error(QString p_err);
    
public slots:
    
private slots:
    void _timerFire();
    void _cmdReceived(int p_id, OMCommandBuffer* p_cmd);

private:
    OMNetwork* m_net;
    FilmParameters* m_film;
    OMAxis* m_master;
    QTimer* m_timer;
    GlobalOptions* m_gopts;

        // commands are associated either as a run state check (false), or run time check (true)
    QHash<int, bool> m_cmds;

    bool m_started;
    bool m_running;

    bool m_runStat;
    unsigned long m_runTime;
    int m_gotCount;



};

#endif // PLAYMONITOR_H
