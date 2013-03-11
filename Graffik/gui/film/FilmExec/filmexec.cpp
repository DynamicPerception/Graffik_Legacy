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

#include "filmexec.h"

#include <QDebug>

FilmExec::FilmExec(OMNetwork* c_net, FilmParameters* c_params, AxisOptions* c_opts, GlobalOptions *c_gopts, QObject *parent) : QObject(parent) {
       m_net = c_net;
    m_params = c_params;
      m_opts = c_opts;
     m_gopts = c_gopts;

        // get an initial copy of film parameters
    m_film = m_params->getParamsCopy();

           m_stat = FILM_STOPPED;
        m_shuttle = SHUTTLE_NONE;
    m_filmPrepped = false;
          m_check = false;

        // initialize our home position monitor (we'll need this later)
      m_position = new PositionMonitor(m_net, m_gopts);
    m_homeThread = new QThread();

    m_position->moveToThread(m_homeThread);
    m_homeThread->start();

        // initialize play status monitor
          m_play = new PlayMonitor(m_net, m_params, m_gopts);
    m_playThread = new QThread();

    m_play->moveToThread(m_playThread);
    m_playThread->start();


    connect(m_net, SIGNAL(complete(int,OMCommandBuffer*)), this, SLOT(_cmdReceived(int,OMCommandBuffer*)), Qt::QueuedConnection);

    connect(m_position, SIGNAL(allAtPosition()), this, SLOT(_nodesAtPosition()), Qt::QueuedConnection);
    connect(m_position, SIGNAL(error(QString)), this, SLOT(_error(QString)), Qt::QueuedConnection);

        // capture signal here..
    connect(m_play, SIGNAL(playStatus(bool,ulong)), this, SLOT(_playStatus(bool,ulong)), Qt::QueuedConnection);

    connect(m_play, SIGNAL(error(QString)), this, SLOT(_error(QString)), Qt::QueuedConnection);

}


FilmExec::~FilmExec() {

    m_position->stop();
    m_homeThread->quit();
    m_homeThread->wait();

    delete m_position;
    delete m_homeThread;

    m_play->stop();
    m_playThread->quit();
    m_playThread->wait();

    delete m_play;
    delete m_playThread;

}

 /* Do all preparatory activities for executing a film
    Can pass an argument as to whether or not nodes should be
    sent home (if required).

    As this method is often called by methods triggered from
    signals/slots, it does not throw an exception, instead uses
    a special return value.  This is to prevent the main application
    from having to handle all exceptions on its own.  Obviously, this
    is a problem from a design perspective, but it avoids unnatural
    acts related to Qt.

    Possible return values are:

    FILM_OK_HOME - prep'd ok, nodes sent home
    FILM_OK - prep'd ok, nodes not sent home
    FILM_ERR_MASTER - no master was found

 */

int FilmExec::_prepFilm(bool p_home) {

    qDebug() << "FEx: Prep Film";

        // refresh film parameters for a fresh start
    m_film = m_params->getParamsCopy();

        // deal with homing requirements
    bool sentHome = false;
    m_axesHome.clear();

        // find axes and timing master
    QList<OMAxis*>  axes = _getAxes(&m_film);
    OMAxis* timingMaster = _getTimingMaster(&axes);

    if( timingMaster == 0 ) {
        QString errText = "No timing master has been assigned, film cannot Be started. Select the camera enabled for at least one axis.";
        emit error(errText);
        return FILM_ERR_MASTER;
    }

    qDebug() << "FEx: Got Master: " << timingMaster;

        // send all axes home and prep their movements, if they are configured for movement
    foreach(OMAxis* axis, axes) {

        unsigned short addr = axis->address();
        long distanceToMove = abs(m_film.axes.value(addr)->endDist);
        bool           mute = m_film.axes.value(addr)->mute;

        if( distanceToMove != 0 && ! mute ) {

                // only do this if moving and if requested to allow
                // send nodes to home

            if( p_home ) {
                qDebug() << "FEx: Sending Device Home: " << addr;
                _sendHome(&m_film, axis);

                // record that we sent axes home
                m_axesHome.insert(axis, 0);
                sentHome = true;
            }

            qDebug() << "FEx: Sending Node Movements";
            _sendNodeMovements(&m_film, axis);

        }
        else {
                // motor is not moving in this film, so disable it
            qDebug() << "FEx: Disabling Motor";
            _disableMotor(axis);
        }

    }

    // when starting from a stopped state, we transmit timing,
    // configuration, and movement data

    _sendCamera(timingMaster);
    _sendMaster(timingMaster, axes);
    m_play->master(timingMaster);

    if( sentHome )
        return FILM_OK_HOME;
    else
        return FILM_OK;

}


 /** Check If Film Running

   This method is primarily used to determine if a film is already executing,
   perhaps not started by this instance.  If it is running, the filmStarted()
   signal will be emitted, and you will begin to see filmPlayStatus() signals
   emitted.

   */

void FilmExec::checkRunning() {

    m_check = true;

        // refresh film parameters for a fresh start
    m_film = m_params->getParamsCopy();

        // find axes and timing master
    QList<OMAxis*>  axes = _getAxes(&m_film);
    OMAxis* timingMaster = _getTimingMaster(&axes);

    if( timingMaster == 0 ) {
        qDebug() << "FEx: checkRunning: No Master, bailing out.";
        return;
    }

    m_play->master(timingMaster);

    m_play->start(true);

}

 /** Advance One Frame in a Shoot-Move-Shoot Sequence

   If the film is set as a shoot-move-shoot style film, this method
   allows you to do a frame advance.  This method must be called at
   least once before calling frameRewind.

   */

void FilmExec::frameAdvance() {

        // refresh film parameters for a fresh start
    m_film = m_params->getParamsCopy();

    if( m_film.filmMode != FILM_MODE_SMS )
        return;

    if( ! m_filmPrepped ) {
        int ret = _prepFilm(false);

        if( ret == FILM_ERR_MASTER ) {
            qDebug() << "FEx: frameAdvance: No Master Found";
         return;
        }

        m_filmPrepped = true;

    }

    QList<OMAxis*>  axes = _getAxes(&m_film);

    foreach(OMAxis* axis, axes) {

        unsigned short addr = axis->address();
        long distanceToMove = abs(m_film.axes.value(addr)->endDist);
        bool           mute = m_film.axes.value(addr)->mute;

        if( distanceToMove != 0 && ! mute ) {
            axis->planAdvance();
        }
    }


}

/** Reverse One Frame in a Shoot-Move-Shoot Sequence

  If the film is set as a shoot-move-shoot style film, this method
  allows you to do a frame rewind.

  Will do nothing if the film is not an SMS type, or frameAdvance() has
  not been called at least once since the last call to stop.

  */

void FilmExec::frameReverse() {
        // refresh film parameters for a fresh start
    m_film = m_params->getParamsCopy();

    if( m_film.filmMode != FILM_MODE_SMS )
        return;

    if( ! m_filmPrepped )
        return;

    QList<OMAxis*>  axes = _getAxes(&m_film);

    foreach(OMAxis* axis, axes) {

        unsigned short addr = axis->address();
        long distanceToMove = abs(m_film.axes.value(addr)->endDist);
        bool           mute = m_film.axes.value(addr)->mute;

        if( distanceToMove != 0 && ! mute ) {
            axis->planReverse();
        }
    }
}

 /** Start, or Resume, Execution of the Film

    If program is currently stopped, all motors will be sent to their
    home position before starting, and all parameters for film execution
    will be transmitted to required nodes before a broadcast start is
    issued.  Note that if nodes are to be sent home, actual starting of
    the film is not immediate.  Instead, the nodes must reach home first -
    in this case, attach a slot to the filmPlayStatus() signal and monitor
    for the running state.

    If already started, this method is non-operative.

    If paused, all nodes will be sent a broadcast start without first
    sending nodes to home.

   */

void FilmExec::start() {

    if( m_stat == FILM_STARTED )
        return;



    qDebug() << "FEx: Start Film";

    bool sentHome = false;
    bool   doHome = false;
        m_shuttle = SHUTTLE_NONE;

        // If stopped, we have to do several things before
        // starting - but if paused, we can just broadcast
        // a start command.

    if( m_stat == FILM_STOPPED ) {

            // if we're starting from a stopped state:
            // we always send a stop, just in case..

        stop();

            // .. and nodes are sent to home position
        doHome = true;
    }

        // see comments for _prepFilm()
    int ret = _prepFilm(doHome);

    if( ret == FILM_ERR_MASTER ) {
            qDebug() << "FEx: Got error for no master, returning";
            return;
    }

    sentHome = ret == FILM_OK_HOME ? true : false;

        // send broadcast command if we didn't send nodes
        // home.

    if( ! sentHome ) {
        m_net->broadcast(OMBus::OM_BCAST_START);
        m_stat = FILM_STARTED;
        m_play->start();

        emit filmStarted();
    }
    else {
            // if we sent nodes home, update HomeMonitor with
            // list of axes sent home
       m_position->checkAxes(m_axesHome);
       m_position->start();
       m_shuttle = SHUTTLE_HOME;
    }
}


void FilmExec::stop() {

    m_position->stop();
    m_play->stop();
    m_net->broadcast(OMBus::OM_BCAST_STOP);

           m_stat = FILM_STOPPED;
    m_filmPrepped = false;

}

void FilmExec::pause() {

    if( m_stat == FILM_PAUSED )
        return;

    m_net->broadcast(OMBus::OM_BCAST_PAUSE);

    m_stat = FILM_PAUSED;
}


/** Rewind Film

  In most cases, perfoms a simple activity:

  Sends all nodes home.

  Does not send a node to start point if it has no distance to move
  or the axis is muted.

  When all axes have arrived at their target position, the shuttleComplete() signal will
  be emitted.

  */

void FilmExec::rewind() {

        // refresh film parameters for a fresh start
    m_film = m_params->getParamsCopy();

    QList<OMAxis*> axes = _getAxes(&m_film);
    m_axesHome.clear();

    foreach(OMAxis* axis, axes) {

        unsigned short          addr = axis->address();
        unsigned long distanceToMove = m_film.axes.value(addr)->endDist;
        bool                    mute = m_film.axes.value(addr)->mute;

        if( distanceToMove != 0 && ! mute ) {
                // only do this if moving
            qDebug() << "FEx: Sending Device Home: " << addr;

            _sendHome(&m_film, axis);

                // record that we sent axes home
            m_axesHome.insert(axis, 0);
        }

    }

    if( m_axesHome.count() < 1 ) {
        emit shuttleComplete();
        return;
    }

    m_position->checkAxes(m_axesHome);
    m_position->start();
    m_shuttle = SHUTTLE_BEG;

}

/** Fast Foward Film

  In most cases, performs a simple activity:

  Sends all nodes to their end positions.

  Does not send a node to end point if it has no distance to move
  or the axis is muted.

  When all axes have arrived at their target position, the shuttleComplete() signal will
  be emitted.

  */

void FilmExec::ffwd() {

        // refresh film parameters for a fresh start
    m_film = m_params->getParamsCopy();

        // We must first determine location of nodes, so that we may send them to their desired position...
    QList<OMAxis*> axes = _getAxes(&m_film);
    m_axesHome.clear();

        // do this first...
    m_shuttle = SHUTTLE_END;

        // Send commands to read current position from each
        // node which shall move, so we can determine how
        // far, and in which direction, to move to reach their
        // end destinations.

    foreach(OMAxis* axis, axes) {

        unsigned short addr = axis->address();
        long         moveTo = m_film.axes.value(addr)->endDist;
        bool           mute = m_film.axes.value(addr)->mute;

        if( moveTo != 0 && ! mute ) {
                // only do this if moving
            qDebug() << "FEx: Querying Device Position: " << addr;

                // set maximum step speed
            OMaxisOptions*    aopts = m_opts->getOptions(addr);
            axis->maxStepSpeed(aopts->maxSteps);

                // get distance fro home
            int cmdId = axis->getHomeDist();
            m_net->getManager()->hold(cmdId);
            m_cmds.insert(cmdId, axis);

                // record that we want axis to go to a specific position
            m_axesHome.insert(axis, moveTo);
        }

    } //end foreach

    if( m_axesHome.count() < 1 ) {
        emit shuttleComplete();
        return;
    }

    m_position->checkAxes(m_axesHome);


}

/** Send Multiple Axes to a Specific Position

  For the listed axes, send each axis to a specified position.

  Pass a QHash with each key being the node address, and the value
  being a long integer with the specified target direction (including
  direction, so negative or positive integers).  For any axis which is not
  muted, the axis will be sent to that position.

  When all axes have arrived at their target position, the shuttleComplete()
  signal will be issued.

  @param p_positions
  Each axis to move, and the position to move it to
  */

void FilmExec::sendAxesTo(QHash<unsigned short, long> p_positions) {

    qDebug() << "FEx: Send Axes to Position";

    // refresh film parameters for a fresh start
    m_film = m_params->getParamsCopy();

    // We must first determine location of nodes, so that we may send them to their desired position...
    QList<OMAxis*> axes = _getAxes(&m_film);
    m_axesHome.clear();
    m_shuttleTo.clear();

    // do this first...
    m_shuttle = SHUTTLE_POS;

    // Send commands to read current position from each
    // node which shall move, so we can determine how
    // far, and in which direction, to move to reach their
    // end destinations.

    foreach(OMAxis* axis, axes) {

        unsigned short addr = axis->address();
        long         moveTo = p_positions.value(addr);
        bool           mute = m_film.axes.value(addr)->mute;

        if( ! p_positions.contains(addr) )
            continue;

        if( moveTo != 0 && ! mute ) {

                // only do this if moving
            qDebug() << "FEx: Querying Device Position: " << addr;

                // set maximum step speed
            OMaxisOptions*    aopts = m_opts->getOptions(addr);
            axis->maxStepSpeed(aopts->maxSteps);

                // request home distance

            int cmdId = axis->getHomeDist();
            m_net->getManager()->hold(cmdId);
            m_cmds.insert(cmdId, axis);

                // record that we sent axes home
            m_axesHome.insert(axis, moveTo);
            m_shuttleTo.insert(addr, moveTo);
        }

    } //end foreach

    if( m_axesHome.count() < 1 ) {
        emit shuttleComplete();
        return;
    }

    m_position->checkAxes(m_axesHome);

}

/** Return Current Execution Status

  Returns one of the following:

  FILM_STOPPED
  FILM_PAUSED
  FILM_STARTED

  @return
  Current status code
  */

int FilmExec::status() {

    return m_stat;
}

 /** Get run (wallclock) time
   */

unsigned long FilmExec::runTime() {
    return m_film.realLength;
}

 /** Get film length
   */

unsigned long FilmExec::filmTime() {
    return m_film.length;
}


/** Return Minimum Achievable Interval

  Returns the minimum achievable interval based on current film
  parameters, in milliseconds.

  This method is static

  @param p_film
  A pointer to the OMfilmParams to analyze

  @return
  The minimum achievable interval
  */

unsigned long FilmExec::minInterval(OMfilmParams *p_film) {

    unsigned long  shutMs = p_film->camParams->shutterMS;
    unsigned long delayMs = p_film->camParams->delayMS;
    unsigned long   focMs = p_film->camParams->focusMS;
    bool            focus = p_film->camParams->focus;

    unsigned long    ival = shutMs + delayMs;

    if( focus )
        ival += focMs;

    return(ival);
}

/** Return the Actual Interval for the Supplied Film Parameters

  Given a reference to a film parameters structure, returns the
  actual interval time that will be used - based on the
  minimum achievable interval, user-supplied interval, or automatically-
  determined interval.  The source of the interval is based on the
  configured parameters, this method automatically determines the
  best interval based on these parameters.

  This method is static

  @param p_film
  A pointer to the OMfilmParams to analyze

  @return
  The interval to be used

  */

unsigned long FilmExec::interval(OMfilmParams* p_film) {

    bool         autoFPS = p_film->camParams->autoFPS;
    bool          manInt = p_film->camParams->manInterval;
    int              fps = p_film->fps;
    unsigned long     iv = p_film->camParams->interval;
    unsigned long filmTm = p_film->length;
    unsigned long realTm = p_film->realLength;

    // determine minimum amount of interval
    // time based on input values...

    unsigned long minInt = minInterval(p_film);

    if( manInt && iv < minInt ) {
            // manual interval, lock to minimum achievable interval
        iv = minInt;
    }
    else if( autoFPS ) {
            // auto fps - determine interval from
            // film length
        iv = (float) realTm / ((float) filmTm / (float) fps);

        if( iv < minInt )
            iv = minInt;

    }
    else if( ! manInt ) {
            // only FPS specified, interval is minimum interval
        iv = minInt;
    }


    return iv;

}

 /* Transmit Functions */

void FilmExec::_sendHome(OMfilmParams *p_film, OMAxis* p_axis) {
    qDebug() << "FEx: Sending node home" << p_axis->address();

    OMfilmAxisParams* parms = p_film->axes.value(p_axis->address());
    OMaxisOptions*    aopts = m_opts->getOptions(p_axis->address());

    p_axis->maxStepSpeed(aopts->maxSteps);
    p_axis->motorEnable();
    p_axis->easing(parms->easing);
    p_axis->microSteps(1);
    p_axis->home();

}

void FilmExec::_sendDistance(OMAxis *p_axis, unsigned long p_distance, bool p_dir) {
    qDebug() << "FEx: Sending node to position" << p_axis->address() << p_distance << p_dir;

    OMaxisOptions*    aopts = m_opts->getOptions(p_axis->address());

    p_axis->maxStepSpeed(aopts->maxSteps);
    p_axis->motorEnable();
    p_axis->microSteps(1);
    p_axis->move(p_dir, p_distance);
}

void FilmExec::_sendMaster(OMAxis *p_master, QList<OMAxis *> p_axes) {
        // I choose you!
    p_master->timing(true);
        // send max run time
    p_master->maxRunTime( m_params->getParamsCopy().realLength );

        // inform all of the slaves that they, well, are slaves.
    foreach(OMAxis* axis, p_axes) {
        if(axis != p_master) {
            qDebug() << "Sending slave state to node" << axis->address();
            axis->timing(false);
        }
    }
}

void FilmExec::_sendCamera(OMAxis* p_master) {

    qDebug() << "FEx: Send Camera Params" << p_master->address();

    bool  camControl = m_film.camParams->camControl;
    unsigned long iv = interval(&m_film);

    p_master->interval(iv);

    if( ! camControl ) {
        p_master->cameraDisable();
        return;
    }

    p_master->cameraEnable();
    p_master->exposure(m_film.camParams->shutterMS);
    p_master->exposureDelay(m_film.camParams->delayMS);

    if( m_film.camParams->focus )
        p_master->focus(m_film.camParams->focusMS);
    else
        p_master->focus(0);


}


void FilmExec::_disableMotor(OMAxis *p_axis) {
    p_axis->motorDisable();
}

void FilmExec::_sendNodeMovements(OMfilmParams *p_film, OMAxis *p_axis) {

        // send movement parameters to axis node

    unsigned short     addr = p_axis->address();
    OMfilmAxisParams* parms = p_film->axes.value(addr);
    bool              which = p_film->filmMode == FILM_MODE_SMS ? true : false;
    long                end = parms->endDist;
    bool                dir = end < 0 ? false : true;
    // if no end time specified, arrive at end of film
    unsigned long    arrive = parms->endTm > 0 ? parms->endTm : p_film->realLength;
    unsigned long     accel = parms->accelTm;
    unsigned long     decel = parms->decelTm;
    unsigned long     start = parms->startTm;
    OMaxisOptions*    aopts = m_opts->getOptions(addr);

        // multiply end point by microsteps, we jog in rapid
    end = end * parms->ms;
    end = end < 0 ? end * -1 : end;

        // we store when the movement ends, but the
        // value we need to transmit is how long the total
        // move takes, so we subtract any start time from it
    arrive = arrive - parms->startTm;

    if( which ) {
        // in sms mode, we go by shots - not walltime
        unsigned long iv = interval(&m_film);

        float start_shots = start / iv;

        start_shots = _round(start_shots);
              start = start_shots;

        float total_shots = arrive / iv;

        total_shots = _round(total_shots);
             arrive = total_shots + 1;


        float ac_shots = accel / iv;

        ac_shots = _round(ac_shots);
           accel = ac_shots;

        float dc_shots = decel / iv;

        dc_shots = _round(dc_shots);
           decel = dc_shots;
    }

    qDebug() << "FE: Motor Params: " << which << dir << start << end << arrive << accel << decel << aopts->backlash;

    p_axis->maxStepSpeed(aopts->maxSteps);
    p_axis->motorEnable();
    p_axis->autoPause(false); // always disable autopause
    p_axis->backlash(aopts->backlash);
    p_axis->continuous(false);
    p_axis->delayMove(start);
    p_axis->easing(parms->easing);
        // We do this after the node gets home!
    p_axis->plan(which, dir, end, arrive, accel, decel);

}


// TODO: Fix timing master check

OMAxis* FilmExec::_getTimingMaster(QList<OMAxis *> *p_axes) {
    foreach(OMAxis* axis, *p_axes) {
        OMaxisOptions* aopts = m_opts->getOptions(axis->address());

        if( aopts->master == true )
            return axis;

    }

    return 0;
}


QList<OMAxis*> FilmExec::_getAxes(OMfilmParams* p_film) {
    QList<OMAxis*> ret;


        // go through list of axes
    foreach( unsigned short addr, p_film->axes.keys() ) {

        OMDevice* axis;

        try {
            axis = m_net->deviceInfo(p_film->axes.value(addr)->bus, addr)->device;
        }
        catch (int e) {
            qDebug() << "FEx: Got exception getting device info: " << e;
            return ret;
        }

        if( axis->type() == "nanoMoCo" ) {
                // ensure that we have the right type, and cast to
                // OMAxis
            OMAxis* omaxis = dynamic_cast<OMAxis*>(axis);

            if ( omaxis != 0 )
                ret.append(omaxis);
        }
    }

    return ret;
}


    // this slot is called by HomeManager when
    // all nodes arrive at home.  We broadcast a
    // start when all nodes are found to be at home

void FilmExec::_nodesAtPosition() {
    qDebug() << "FEx: Nodes are at desired position";

        // if shuttling home, it's because we started a
        // film...  Otherwise, we're just moving to start or end positions

    if( m_shuttle == SHUTTLE_HOME ) {

            // send mS parameter for each node - do this after moving
            // to ensure home uses rapid moves
        foreach(OMAxis* axis, m_axesHome.keys())
            axis->microSteps(m_film.axes.value(axis->address())->ms);


        qDebug() << "FEx: Nodes all re-configured, starting";

        m_stat = FILM_STARTED;

        m_net->broadcast(OMBus::OM_BCAST_START);
        m_position->stop();
        m_play->start();
        emit filmStarted();
    }
    else {
        m_position->stop();
        emit shuttleComplete();
    }
}


float FilmExec::_round(float p_val) {
    p_val = p_val > int(p_val) ? int(p_val) + 1 : int(p_val);
    return p_val;
}

void FilmExec::_error(QString p_error) {
    m_position->stop();
    m_play->stop();

    emit error(p_error);
}


void FilmExec::_cmdReceived(int p_id, OMCommandBuffer *p_cmd) {
    // received command completed from network manager

        // ignore any command we're not monitoring
    if( ! m_cmds.contains(p_id) )
        return;

    qDebug() << "FEx: Wanted Command" << p_id;

    if( p_cmd->status() == OMC_SUCCESS ) {
        unsigned int resSize = p_cmd->resultSize();

        if( resSize > 0 ) {

            char* res = new char[resSize];
            p_cmd->result(res, resSize);

            if( m_shuttle == SHUTTLE_END || SHUTTLE_POS ) {

                    // for shuttle to end status, we need to
                OMAxis* axis = m_cmds.value(p_id);
                unsigned short addr = axis->address();

                    // get target position for axis, and determine
                    // distance from target
                long   target = m_film.axes.value(addr)->endDist;
                long position = qFromBigEndian<qint32>((uchar*)res);
                long distance = 0;
                bool      dir = true;

                    // for shuttling to a specific position (vs. end), we want the
                    // target position, not the move complete position
                if( m_shuttle == SHUTTLE_POS )
                    target = m_shuttleTo.value(addr);

                if( target > position ) {
                    distance = target - position;
                    dir = true;
                }
                else {
                    distance = position - target;
                    dir = false;
                }


                qDebug() << "FEx: Got Position Response for Axis" << m_cmds.value(p_id)->address() << ":" << position << target << distance << dir;

                _sendDistance(axis, distance, dir);
            } // end if shuttle_end

            delete res;

        } // end if( resSize > 0...
    } // end if success
    else if( m_gopts->stopOnErr() ) {
        QString errText = "Received Error Sending Device " + m_net->getDevices().value(m_cmds.value(p_id)->address())->name + " request for position";
        _error(errText);
    }

    m_cmds.remove(p_id);
    m_net->getManager()->release(p_id);

        // no more axes left, we can tell the monitor
        // to start checking to see if all nodes are at
        // their target position (for shuttle to end)
    if( m_cmds.count() < 1 && ( m_shuttle == SHUTTLE_END || m_shuttle == SHUTTLE_POS ) )
        m_position->start();

}


void FilmExec::_playStatus(bool p_stat, unsigned long p_time) {
    if( m_check == true ) {
        m_check = false;

        if( p_stat == true )
            emit filmStarted();
    }

    emit filmPlayStatus(p_stat, p_time);
}
