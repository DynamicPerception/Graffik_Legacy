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


    QList<OMAxis*> axes = _getDevices(&params);

        // If stopped, we have to do several things before
        // starting - but if paused, we can just broadcast
        // a start command.

    if( m_stat == FILM_STOPPED ) {

            // if we're starting from a stopped state:

            // refresh film parameters for a fresh start
        m_film = m_params->getParamsCopy();


            // send all axes home
        foreach(OMAxis* axis, axes) {
            unsigned short addr = axis->address();
            unsigned long distanceToMove = abs(params.axes.value(addr)->endDist);
            if( distanceToMove > 0 )
                _sendHome(axis);
        }

            // when starting from a stopped state, we transmit timing,
            // configuration, and movement data

            // TODO: Check for failure

        _sendTiming();
        _sendConfig();
        _sendMovements();
    }

    // send broadcast command
    int cmdId = m_net->broadcast(OM_BCAST_START);

    m_stat = FILM_STARTED;
}


void FilmExec::stop() {

    if( m_stat == FILM_STOPPED )
        return;

    int cmdId = m_net->broadcast(OM_BCAST_STOP);

    m_stat = FILM_STOPPED;

}

void FilmExec::pause() {

    if( m_stat == FILM_PAUSED )
        return;

    int cmdId = m_net->broadcast(OM_BCAST_PAUSE);

    m_stat = FILM_PAUSED;
}

void FilmExec::status() {

    return m_stat;
}

void FilmExec::runTime() {

}

void FilmExec::filmTime() {

}



 /* Transmit Functions */

void FilmExec::_sendAllHome() {


}

void FilmExec::_sendConfig() {

}

void FilmExec::_sendTiming() {

}

void FilmExec::_sendNodeMovements() {

}





QList<OMAxis*> FilmExec::_getDevices(OMfilmParams* p_params) {
    QList<OMAxis*> ret;


    foreach(unsigned short addr, params->axes)
        ret.append( m_net->deviceInfo(params->axes.value(addr)->bus, addr) );

    m_params->releaseParams();

    return ret;
}
