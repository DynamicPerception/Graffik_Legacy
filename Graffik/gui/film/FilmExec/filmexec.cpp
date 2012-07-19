#include "filmexec.h"

FilmExec::FilmExec(OMNetwork* c_net, FilmParameters* c_params) : QObject() {
    m_net = c_net;
    m_params = c_params;

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

            // refresh film parameters for a fresh start
        m_film = m_params->getParamsCopy();

        QList<OMAxis*> axes = _getAxes(&m_film);

            // send all axes home
        foreach(OMAxis* axis, axes) {
            unsigned short addr = axis->address();
            unsigned long distanceToMove = abs(m_film.axes.value(addr)->endDist);
            if( distanceToMove > 0 )
                _sendHome(axis);
        }

            // when starting from a stopped state, we transmit timing,
            // configuration, and movement data

            // TODO: Check for failure

        _sendTiming();
        _sendConfig();
        _sendNodeMovements();
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

int FilmExec::status() {

    return m_stat;
}

unsigned long FilmExec::runTime() {

}

unsigned long FilmExec::filmTime() {

}



 /* Transmit Functions */

void FilmExec::_sendHome(OMAxis* p_axis) {
    int cmdId = p_axis->home();
}

void FilmExec::_sendConfig() {

}

void FilmExec::_sendTiming() {

}

void FilmExec::_sendNodeMovements() {

}





QList<OMAxis*> FilmExec::_getAxes(OMfilmParams* p_film) {
    QList<OMAxis*> ret;


        // go through list of axes
    foreach( unsigned short addr, p_film->axes.keys() ) {
        OMDevice* axis = m_net->deviceInfo(p_film->axes.value(addr)->bus, addr)->device;

        if( axis->type() == "OpenMoCo Axis" ) {
                // ensure that we have the right type, and cast to
                // OMAxis
            OMAxis* omaxis = dynamic_cast<OMAxis*>(axis);

            if ( omaxis != 0 )
                ret.append(omaxis);
        }
    }

    m_params->releaseParams();

    return ret;
}
