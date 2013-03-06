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

#include "playmonitor.h"


#include <QDebug>
#include <QtEndian>

PlayMonitor::PlayMonitor(OMNetwork *c_net, FilmParameters *c_film, GlobalOptions *c_gopts, QObject *parent) : QObject(parent) {
    m_film      = c_film;
    m_net       = c_net;
    m_gopts     = c_gopts;
    m_started   = false;
    m_checkOnce = false;

        // listen for commands completed
    connect(m_net, SIGNAL(complete(int,OMCommandBuffer*)), this, SLOT(_cmdReceived(int,OMCommandBuffer*)), Qt::QueuedConnection);

}

PlayMonitor::~PlayMonitor() {
    if( m_started )
        delete m_timer;
}

/** Start Timer

   If the checkOnce flag is set to true, the timer will automatically stop if
   a film isn't running.  This can be used to determine if a film is still
   executing when starting the program, for example
*/

void PlayMonitor::start(bool p_checkOnce) {

    m_checkOnce = p_checkOnce;

    if( ! m_started ) {
        m_timer = new QTimer();
        connect(m_timer, SIGNAL(timeout()), this, SLOT(_timerFire()));
        m_timer->start(PM_TIMER_PERIOD);
        m_started = true;
    }

}

/** Stop Timer */

void PlayMonitor::stop() {
    if( m_started == true ) {
        m_timer->stop();
        delete m_timer;
        m_started = false;
    }
}


/** Specify master node

    You -must- specify the master before starting the timer!
*/

void PlayMonitor::master(OMAxis *p_master) {
    m_master = p_master;
}

void PlayMonitor::_timerFire() {

        // on timer fire, we issue a command to the
        // master device to request its run status, and
        // another to request its run time.

        // don't do anything if a master isn't set..
    if( m_master == 0 )
        return;


    int cmdId = m_master->getRunning();
    m_net->getManager()->hold(cmdId);
    m_cmds.insert(cmdId, false);

    cmdId = m_master->getRunTime();
    m_net->getManager()->hold(cmdId);
    m_cmds.insert(cmdId, true);

    m_gotCount = 0;
    m_runStat  = false;
    m_runTime  = 0;

}

void PlayMonitor::_cmdReceived(int p_id, OMCommandBuffer *p_cmd) {

    if( ! m_cmds.contains(p_id) )
        return;

   // qDebug() << "PM: Recv'd back " << p_id;

    if( p_cmd->status() == OMC_SUCCESS ) {
        unsigned int resSize = p_cmd->resultSize();

        if( resSize > 0 ) {

            char* res = new char[resSize];
            p_cmd->result(res, resSize);

                // place value in right place..
            if( m_cmds.value(p_id) == false ) {
                m_runStat = res[0];
            }
            else {
                m_runTime = qFromBigEndian<quint32>((uchar*)res);
            }

            delete res;
            m_gotCount++;

        } // end if( resSize > 0...
    }
    else {

        qDebug() << "PM: Got Err" << p_cmd->status();

        if( m_gopts->stopOnErr() ) {
            QString errText = "Received Error Monitoring Master Device " + m_net->getDevices().value(m_master->address())->name + " " + QString::number(p_cmd->status()) + ", Film Aborted";
            emit error(errText);
        }

    }

    m_cmds.remove(p_id);
    m_net->getManager()->release(p_id);

    if( m_gotCount > 1 ) {

        if( m_checkOnce == true ) {
                // if the checkonce flag is set...
            m_checkOnce = false;
            if( m_runStat == false ) {
                    // .. and the program is not running, then
                    // stop the timer and bug out.
                stop();
                return;
            }

        }

        emit playStatus(m_runStat, m_runTime);
    }


}
