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
