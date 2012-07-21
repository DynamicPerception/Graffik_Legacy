#ifndef FILMEXEC_H
#define FILMEXEC_H

#include <QObject>
#include <QString>
#include <QList>

#include "MoCoBus/omnetwork.h"
#include "Devices/nanoMoCo/omaxis.h"

#include "film/FilmParameters/filmparameters.h"
#include "core/AxisOptions/axisoptions.h"


enum { FILM_STOPPED, FILM_STARTED, FILM_PAUSED };


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
    void filmStart(bool p_stat);
    void filmError(QString p_err);

private:
    OMNetwork* m_net;
    FilmParameters* m_params;
    OMfilmParams m_film;
    AxisOptions* m_opts;

    int m_stat;

    void _sendHome(OMAxis* p_axis);
    void _sendCamera(OMAxis *p_master);
    void _sendConfig();
    void _sendNodeMovements();

    QList<OMAxis*> _getAxes(OMfilmParams *p_film);
    OMAxis* _getTimingMaster(QList<OMAxis*>* p_axes);


};

#endif // FILMEXEC_H
