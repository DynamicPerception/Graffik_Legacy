#include "filmexec.h"

FilmExec::FilmExec(OMNetwork* c_net, FilmParameters* c_film) : QObject() {
    m_net = c_net;
    m_params = c_film;

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
    else if(m_stat == FILM_STOPPED )
        // go home
        _sendAllHome();


    _sendStart();




}

void FilmExec::stop() {

}

void FilmExec::pause() {

}

void FilmExec::status() {

}

void FilmExec::runTime() {

}

void FilmExec::filmTime() {

}

void FilmExec::_sendStart() {

}

void FilmExec::_sendAllHome() {

}
