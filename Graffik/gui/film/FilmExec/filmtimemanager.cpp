#include "filmtimemanager.h"

#include <QDebug>

FilmTimeManager::FilmTimeManager(FilmExec *c_exec, FilmParameters *c_params, QObject *parent) : QObject(parent) {

    m_exec     = c_exec;
    m_params   = c_params;
    m_curTime  = 0;
    m_filmTime = m_params->getParamsCopy().realLength;

    connect(m_exec, SIGNAL(filmPlayStatus(bool,ulong)), this, SLOT(playStatus(bool,ulong)));
    connect(m_params, SIGNAL(paramsReleased()), this, SLOT(filmParamsChanged()));

}

FilmTimeManager::~FilmTimeManager() {

}

/** Set Current Film Time Position

  Sets the current film time position to the specified value and emits
  the timePositionChanged signal.

  If the specified time is greater than the real time length of the film, or
  the current time is the same as the specified value, this method does nothing.

  @param p_time
  The current (real) film time in mS
  */

void FilmTimeManager::timePosition(unsigned long p_time) {

    if( p_time > m_filmTime || m_curTime == p_time )
        return;

    m_curTime = p_time;

    qDebug() << "FTM: Emit time position change" << m_curTime;
    emit timePositionChanged(m_curTime);
}

/** Get Current Film Time Position
  */

unsigned long FilmTimeManager::timePosition() {
    return m_curTime;
}

/** Play Status Update Slot

  This slot is called whenever the playStatus signal is emitted from the
  attached FilmExec object.  The current film time is updated based on the
  value emitted with the signal, if that time is different than the current
  set time, the timePositionChanged() signal is emitted.

  @param p_runStat
  Unused

  @param p_curTime
  The current real film time in mS
  */

void FilmTimeManager::playStatus(bool p_runStat, unsigned long p_curTime) {
    Q_UNUSED(p_runStat);

    if( p_curTime == m_curTime)
        return;

    m_curTime = p_curTime;
    qDebug() << "FTM: Emit time position change" << m_curTime;

    emit timePositionChanged(m_curTime);
}

/** Film Parameters Changed Slot

  This slot is called whenever the film parameters are changed.  If
  the film time has changed, the local film time will be updated. If
  the current film time is greater than the new film time, the current
  film time will be set to the end of the new film time and the
  timePositionChanged signal will be emitted.
  */

void FilmTimeManager::filmParamsChanged() {
    unsigned long newFilmTime = m_params->getParamsCopy().realLength;

    if( newFilmTime != m_filmTime ) {
        m_filmTime = newFilmTime;

        if( m_curTime > m_filmTime ) {
            m_curTime = m_filmTime;
            qDebug() << "FTM: Emit time position change" << m_curTime;
            emit timePositionChanged(m_curTime);
        }

    }
}
