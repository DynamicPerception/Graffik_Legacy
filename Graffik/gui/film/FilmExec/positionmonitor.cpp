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

#include "positionmonitor.h"

#include <QDebug>


PositionMonitor::PositionMonitor(OMNetwork *c_net, GlobalOptions *c_gopts, QObject *parent) : QObject(parent) {
    m_net = c_net;
    m_gopts = c_gopts;

    m_started = false;

        // list for commands completed
    connect(m_net, SIGNAL(complete(int,OMCommandBuffer*)), this, SLOT(_cmdReceived(int,OMCommandBuffer*)), Qt::QueuedConnection);

}

PositionMonitor::~PositionMonitor() {
    if( m_started )
        delete m_timer;
}

void PositionMonitor::start() {

    if( ! m_started ) {
        m_timer = new QTimer();
        connect(m_timer, SIGNAL(timeout()), this, SLOT(_timerFire()));
        m_timer->start(HMM_PERIOD);
        m_started = true;
    }

}

void PositionMonitor::stop() {
    if( m_started ) {
        m_timer->stop();
        delete m_timer;
        m_started = false;
    }
}


void PositionMonitor::checkAxes(QHash<OMAxis *, unsigned long> p_axes) {
    m_axes = p_axes;
    m_positions.clear();

        // record target locations
    foreach(OMAxis* axis, m_axes.keys())
        m_positions.insert(axis->address(), m_axes.value(axis));

}

void PositionMonitor::_timerFire() {

        // on timer fire, we issue commands
        // to request distance from home from all
        // known axes.  We check and remove axes
        // with the command complete slot
    foreach(OMAxis* axis, m_axes.keys()) {
        qDebug() << "HM: Sending request for axis" << axis->address();
        int cmdId = axis->getHomeDist();
            // hold on to results!
        m_net->getManager()->hold(cmdId);
            // track commands issued
        m_cmds.insert(cmdId, axis);
    }

}

void PositionMonitor::_cmdReceived(int p_id, OMCommandBuffer *p_cmd) {
    // received command completed from network manager

    if( ! m_cmds.contains(p_id) )
        return;

    if( p_cmd->status() == OMC_SUCCESS ) {
        unsigned int resSize = p_cmd->resultSize();

        if( resSize > 0 ) {

            char* res = new char[resSize];
            p_cmd->result(res, resSize);

                // get target position for axis, and determine
                // distance from target
            unsigned long target = m_positions.value(m_cmds.value(p_id)->address());
            unsigned long position = qFromBigEndian<qint32>((uchar*)res);
            unsigned long distance = target > position ? target - position : position - target;

            delete res;

            qDebug() << "HM: Found current distance from target for axis" << m_cmds.value(p_id)->address() << "is" << distance;

            if( distance < HMM_SLOP ) {
                    // node is (generally) at target, remove it from list
                    // of axes we're monitoring
                m_axes.remove(m_cmds.value(p_id));
                qDebug() << "HM: Device is (close enough to) desired position";
            }

        } // end if( resSize > 0...
    }
    else if( m_gopts->stopOnErr() ){
        QString errText = "Received Error Sending Device " + m_net->getDevices().value(m_cmds.value(p_id)->address())->name + " Home";
        emit error(errText);
    }

    m_cmds.remove(p_id);
    m_net->getManager()->release(p_id);

        // no more axes left, we can emit a signal
        // informing that all nodes are at home!
    if( m_axes.count() < 1 )
        emit allAtPosition();


}
