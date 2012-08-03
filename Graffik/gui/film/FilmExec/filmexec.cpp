#include "filmexec.h"

#include <QDebug>

FilmExec::FilmExec(OMNetwork* c_net, FilmParameters* c_params, AxisOptions* c_opts) : QObject() {
    m_net = c_net;
    m_params = c_params;
    m_opts = c_opts;

        // get an initial copy of film parameters
    m_film = m_params->getParamsCopy();

    m_stat = FILM_STOPPED;
}


FilmExec::~FilmExec() {

}


 /** Start, or Resume, Execution of the Film

    If program is currently stopped, all motors will be sent to their
    home position before starting, and all parameters for film execution
    will be transmitted to required nodes before a broadcast start is
    issued.

    If already started, this method is non-operative.

    If paused, all nodes will be sent a broadcast start without first
    sending nodes to home.
   */

void FilmExec::start() {

    if( m_stat == FILM_STARTED )
        return;

        // If stopped, we have to do several things before
        // starting - but if paused, we can just broadcast
        // a start command.

    if( m_stat == FILM_STOPPED ) {

            // if we're starting from a stopped state:

        qDebug() << "FEx: Start";

            // refresh film parameters for a fresh start
        m_film = m_params->getParamsCopy();

        QList<OMAxis*> axes = _getAxes(&m_film);

            // send all axes home and prep their movements, if they are configured for movement
        foreach(OMAxis* axis, axes) {
            qDebug() << "FEx: Checking Device";
            unsigned short addr = axis->address();

            qDebug() << "FEx: Rawdist:" << m_film.axes.value(addr)->endDist;

            long distanceToMove = abs(m_film.axes.value(addr)->endDist);

            qDebug() << "FEx: DTM:" << distanceToMove << "ADDR:" << addr;

            if( distanceToMove != 0 ) {
                    // only do this if moving
                qDebug() << "FEx: Sending Device Home" << addr;
                _sendHome(axis);
                qDebug() << "FEx: Sending Node Movements";
                _sendNodeMovements(&m_film, axis);
            }
            else {
               // qDebug() << "FEx: Disabling Motor";
               // _disableMotor(axis);
            }
        }

            // when starting from a stopped state, we transmit timing,
            // configuration, and movement data

            // TODO: Check for failure

        OMAxis* timingMaster = _getTimingMaster(&axes);

        qDebug() << "FEx: Got Master: " << timingMaster;

        _sendCamera(timingMaster);
        _sendMaster(timingMaster, axes);
    }

    // send broadcast command
    int cmdId = m_net->broadcast(OMBus::OM_BCAST_START);

    m_stat = FILM_STARTED;
}


void FilmExec::stop() {

    if( m_stat == FILM_STOPPED )
        return;

    int cmdId = m_net->broadcast(OMBus::OM_BCAST_STOP);

    m_stat = FILM_STOPPED;

}

void FilmExec::pause() {

    if( m_stat == FILM_PAUSED )
        return;

    int cmdId = m_net->broadcast(OMBus::OM_BCAST_PAUSE);

    m_stat = FILM_PAUSED;
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


/** Return the Actual Interval for the Supplied Film Parameters

  Given a reference to a film parameters structure, returns the
  actual interval time that will be used - based on the
  minimum achievable interval, user-supplied interval, or automatically-
  determined interval.  The source of the interval is based on the
  configured parameters, this method automatically determines the
  best interval based on these parameters.

  @param p_film
  A pointer to the OMfilmParams to analyze

  */

unsigned long FilmExec::interval(OMfilmParams* p_film) {
    bool autoFPS    = p_film->camParams->autoFPS;
    bool manInt     = p_film->camParams->manInterval;
    bool focus      = p_film->camParams->focus;

    int fps         = p_film->fps;

    unsigned long shutMs = p_film->camParams->shutterMS;
    unsigned long delayMs = p_film->camParams->delayMS;
    unsigned long focMs   = p_film->camParams->focusMS;

    unsigned long iv      = p_film->camParams->interval;

    unsigned long filmTm   = p_film->length;
    unsigned long realTm   = p_film->realLength;

    // determine minimum amount of interval
    // time based on input values...

    float minInterval = shutMs + delayMs;

    if( focus )
        minInterval += focMs;


    qDebug() << "FEX: IV:" << iv << "MIV:" << minInterval;

    if( manInt && iv < minInterval ) {
            // manual interval, lock to minimum achievable interval
        iv = minInterval;
    }
    else if( autoFPS ) {
            // auto fps - determine interval from
            // film length
        iv = (float) realTm / (float) filmTm / (float) fps;

        if( iv < minInterval )
            iv = minInterval;
    }
    else if( ! manInt ) {
            // only FPS specified, interval is minimum interval
        iv = minInterval;
    }


    return iv;

}

 /* Transmit Functions */

void FilmExec::_sendHome(OMAxis* p_axis) {
    p_axis->motorEnable();
    p_axis->home();
}


void FilmExec::_sendMaster(OMAxis *p_master, QList<OMAxis *> p_axes) {
        // I choose you!
    p_master->timing(true);
        // inform all of the slaves that they, well, are slaves.
    foreach(OMAxis* axis, p_axes) {
        if(axis != p_master)
            axis->timing(false);
    }
}

void FilmExec::_sendCamera(OMAxis* p_master) {

    qDebug() << "FEx: Send Camera Params" << p_master;

    bool camControl = m_film.camParams->camControl;

    unsigned long iv = interval(&m_film);
    p_master->interval(iv);

    if( ! camControl ) {
        int cmdId = p_master->cameraDisable();
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

    unsigned short addr = p_axis->address();
    OMfilmAxisParams* parms = p_film->axes.value(addr);
    bool which = p_film->filmMode == FILM_MODE_SMS ? true : false;

    long end   = parms->endDist;
    bool dir   = end < 0 ? false : true;
    end = abs(end);

        // if no end time specified, arrive at end of film
    unsigned long arrive = parms->endTm > 0 ? parms->endTm : p_film->realLength;

    p_axis->motorEnable();
    p_axis->continuous(false);
    p_axis->delayMove(parms->startTm);
    p_axis->easing(parms->easing);
    p_axis->microSteps(parms->ms);
    p_axis->plan(which, dir, end, arrive, parms->accelTm, parms->decelTm);

}


OMAxis* FilmExec::_getTimingMaster(QList<OMAxis *> *p_axes) {
    foreach(OMAxis* axis, *p_axes) {
        OMaxisOptions* aopts = m_opts->getOptions(axis->address());
        qDebug() << "FEx: AD/AXTYPE: " << axis->address() << aopts->axisType;
        qDebug() << "FEx: MAX: " << aopts->maxSteps;

        if( aopts->master == true ) {
            qDebug() << "FEx: Found Master: " << axis;
            return axis;
        }
    }
}


QList<OMAxis*> FilmExec::_getAxes(OMfilmParams* p_film) {
    QList<OMAxis*> ret;


    qDebug() << "FEx: GA: Get Axes";

        // go through list of axes
    foreach( unsigned short addr, p_film->axes.keys() ) {

        OMDevice* axis;

        qDebug() << "FEx: Bus: " << p_film->axes.value(addr)->bus;

        try {
            axis = m_net->deviceInfo(p_film->axes.value(addr)->bus, addr)->device;
        }
        catch (int e) {
            qDebug() << "FEx: Got exception: " << e;
            return ret;
        }

        qDebug() << "FEx: GA: Checking Axis type: " << axis->type();

        if( axis->type() == "nanoMoCo" ) {
                // ensure that we have the right type, and cast to
                // OMAxis
            qDebug() << "FEx: Found Axis";
            OMAxis* omaxis = dynamic_cast<OMAxis*>(axis);

            if ( omaxis != 0 )
                ret.append(omaxis);
        }
    }

    return ret;
}
