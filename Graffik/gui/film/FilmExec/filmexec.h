#ifndef FILMEXEC_H
#define FILMEXEC_H

#include <QObject>
#include "MoCoBus/omnetwork.h"
#include "film/FilmParameters/filmparameters.h"

enum {
 FILM_STARTED, FILM_STOPPED, FILM_PAUSED
};


/** Film Execution Handler

    Manages the execution of a film, as defined by a FilmParameters object.

    This class is designed as an execution handler, translating the contents
    of a FilmParameters structure into a series of requested bus actions that
    will result in the control of devices on the bus, towards the creation of
    a film.

    @author C. A. Church

    Copyright &copy; 2012 Dynamic Perception LLC

  */

class FilmExec : public QObject
{

    Q_OBJECT

public:
    FilmExec(OMNetwork* c_net, FilmParameters* c_film);
    ~FilmExec();

    void start();
    void stop();
    void pause();

    int status();
    unsigned long runTime();
    unsigned long filmTime();

private:
    OMNetwork* m_net;
    FilmParameters* m_params;

    int m_stat;

    void _sendStart();
    void _sendAllHome();

};

#endif // FILMEXEC_H
