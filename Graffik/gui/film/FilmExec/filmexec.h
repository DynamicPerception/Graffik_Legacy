#ifndef FILMEXEC_H
#define FILMEXEC_H

#include <QObject>
#include <QString>
#include <QList>
#include <QThread>

#include "MoCoBus/omnetwork.h"
#include "Devices/nanoMoCo/omaxis.h"

#include "film/FilmParameters/filmparameters.h"
#include "core/AxisOptions/axisoptions.h"

#include "homemonitor.h"
#include "playmonitor.h"

enum { FILM_STOPPED, FILM_STARTED, FILM_PAUSED };


/** Film Execution Handler

    Manages the execution of a film, as defined by a FilmParameters object.

    This class is designed as an execution handler, translating the contents
    of a FilmParameters structure into a series of requested bus actions that
    will result in the control of devices on the bus, and execute the creation of
    a film.

    @author C. A. Church

    Copyright &copy; 2012 Dynamic Perception LLC

  */

class FilmExec : public QObject
{

    Q_OBJECT

public:
    FilmExec(OMNetwork* c_net, FilmParameters* c_params, AxisOptions* c_opts);
    ~FilmExec();

    void start();
    void stop();
    void pause();

    int status();
    unsigned long runTime();
    unsigned long filmTime();

    unsigned long interval(OMfilmParams* p_film);


signals:
        /** Film Playing Status Signal

          After a film play has been requested, this signal will emit
          approximately once per second, reporting the current run state
          and run time (in wall time, milliseconds) of the film.  This
          signal will continue to be emitted until the stop() method is
          called.

          @param p_stat
          The running state, false = not running, true = running

          @param p_runTime
          The runtime of the film, in milliseconds.
          */
    void filmPlayStatus(bool p_stat, unsigned long p_runTime);

private slots:
    void _nodesHome();

private:
    OMNetwork* m_net;
    FilmParameters* m_params;
    OMfilmParams m_film;
    AxisOptions* m_opts;

    HomeMonitor* m_home;
    PlayMonitor* m_play;
    QThread* m_homeThread;
    QThread* m_playThread;

    int m_stat;

    void _sendHome(OMAxis* p_axis);
    void _sendCamera(OMAxis* p_master);
    void _sendMaster(OMAxis* p_master, QList<OMAxis*> p_axes);
    void _sendNodeMovements(OMfilmParams* p_film, OMAxis* p_axis);
    void _disableMotor(OMAxis* p_axis);

    float _round(float p_val);

    QList<OMAxis*> _getAxes(OMfilmParams* p_film);
    OMAxis* _getTimingMaster(QList<OMAxis*>* p_axes);


};

#endif // FILMEXEC_H
