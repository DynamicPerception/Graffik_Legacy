#include "globaloptions.h"

GlobalOptions::GlobalOptions(QObject *parent) :
    QObject(parent)
{

    m_mutex = new QMutex;
    m_stopErr = true;
    m_display = Options::Imperial;

}

GlobalOptions::~GlobalOptions() {
    delete m_mutex;
}

/** Set Display Type

   Set display type to one of:

   Options::Imperial
   Options::Metric
   Options::Steps

   This method is thread-safe.
   */

void GlobalOptions::display(int p_mode) {

    if( m_display == p_mode )
        return;

    m_mutex->lock();
    m_display = p_mode;
    m_mutex->unlock();

    emit optionsChanged();
}

/** Return current display type

  This method is thread-safe
  */

int GlobalOptions::display() {
    m_mutex->lock();
    int ret = m_display;
    m_mutex->unlock();
    return ret;
}

/** Set Stop on Error Parameter

  This method is thread safe
  */

void GlobalOptions::stopOnErr(bool p_mode) {
    if( m_stopErr == p_mode )
        return;

    m_mutex->lock();
    m_stopErr = p_mode;
    m_mutex->unlock();

    emit optionsChanged();
}

/** Return Stop on Error Parameter

  This method is thread-safe
  */

bool GlobalOptions::stopOnErr() {
    m_mutex->lock();
    bool ret = m_stopErr;
    m_mutex->unlock();
    return ret;
}
