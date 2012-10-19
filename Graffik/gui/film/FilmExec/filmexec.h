/*

  Graffik Motion Control Application

  Copyright (c) 2011-2012 Dynamic Perception

 This file is part of Graffik.

    Graffik is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Graffik is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Graffik.  If not, see <http://www.gnu.org/licenses/>.

    */

#ifndef FILMEXEC_H
#define FILMEXEC_H

#include <QObject>
#include <QString>
#include <QList>
#include <QHash>
#include <QThread>


#include "MoCoBus/omnetwork.h"
#include "Devices/nanoMoCo/omaxis.h"

#include "film/FilmParameters/filmparameters.h"
#include "core/Options/axisoptions.h"
#include "core/Options/globaloptions.h"

#include "positionmonitor.h"
#include "playmonitor.h"

enum { FILM_STOPPED, FILM_STARTED, FILM_PAUSED };

    // differentiation between shuttling home and shuttle to beginning
    // is minial except for expectation of the user and UI, we typically
    // shuttle home when beginning a film, and shuttle to beginning when
    // not playing

enum { SHUTTLE_NONE, SHUTTLE_HOME, SHUTTLE_END, SHUTTLE_BEG };

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
    FilmExec(OMNetwork* c_net, FilmParameters* c_params, AxisOptions* c_opts, GlobalOptions* c_gopts, QObject* parent = 0);
    ~FilmExec();

    void start();
    void stop();
    void pause();

    void rewind();
    void ffwd();

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

        /** Film Started Signal

          This signal is emitted when the broadcast start is actually sent.
          As nodes may need to travel home, or other activities may need to
          occur before the film start is transmitted, this signal informs
          when the actual start occurs
          */

    void filmStarted();

        /** Shuttle Completed Signal

          When Shuttling (fast forward, rewind, etc.) this signal is emitted when
          all axes have arrived at their desired position
          */

    void shuttleComplete();

        /** Error Occurred Signal

          This signal is emitted when an error occurs.
          */

    void error(QString p_err);

private slots:
    void _nodesAtPosition();
    void _error(QString p_error);
    void _cmdReceived(int p_id, OMCommandBuffer* p_cmd);

private:
    OMNetwork* m_net;
    FilmParameters* m_params;
    OMfilmParams m_film;
    AxisOptions* m_opts;
    GlobalOptions* m_gopts;

    PositionMonitor* m_position;
    PlayMonitor* m_play;
    QThread* m_homeThread;
    QThread* m_playThread;

    QHash<OMAxis*, unsigned long> m_axesHome;
    QHash<int, OMAxis*> m_cmds;

    int m_stat;
    int m_shuttle;

    void _sendHome(OMAxis* p_axis);
    void _sendDistance(OMAxis* p_axis, unsigned long p_distance, bool p_dir);
    void _sendCamera(OMAxis* p_master);
    void _sendMaster(OMAxis* p_master, QList<OMAxis*> p_axes);
    void _sendNodeMovements(OMfilmParams* p_film, OMAxis* p_axis);
    void _disableMotor(OMAxis* p_axis);

    float _round(float p_val);

    QList<OMAxis*> _getAxes(OMfilmParams* p_film);
    OMAxis* _getTimingMaster(QList<OMAxis*>* p_axes);


};

#endif // FILMEXEC_H
