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

#include "speedcontrolproxy.h"

#include <QDebug>

SpeedControlProxy::SpeedControlProxy(AxisOptions *c_opts) : QObject() {
    m_optObj       = c_opts;
    m_curDev       = 0;
    m_maxpct       = 1.0;
    m_curDir       = true;
    m_nextSpd      = 0.0;
    m_curSpd       = 0.0;
    m_dampPeriods  = 50;
    m_curPeriod    = 1;
    m_devSelected  = false;
    m_timerStarted = false;
    m_timer        = 0;
    m_curRes       = 1;
    m_curAddr      = 0;
}

SpeedControlProxy::~SpeedControlProxy() {
    stopDampTimer();

    if( m_timer != 0)
        delete m_timer;
}

void SpeedControlProxy::startDampTimer() {

        // do not start timer if already started
    if( m_timerStarted )
        return;

    qDebug() << "SCP: Starting Timer" << QThread::currentThreadId();

    if( m_timer == 0 ) {
        m_timer = new QTimer();
        connect(m_timer, SIGNAL(timeout()), this, SLOT(_timerFire()));
    }

    m_timer->start(SCP_TIME_PERIOD);

    m_timerStarted = true;
    qDebug() << "SCP: Timer Started";
}

void SpeedControlProxy::stopDampTimer() {

    if( m_timerStarted ) {
        qDebug() << "SCP: Stopping Timer" << QThread::currentThreadId();
        m_timer->stop();
        m_timerStarted = false;
    }
}

/** Set Maximum Speed Percentage

  Sets top end of speed range, specify maximum speed as percentage
  of real max speed (as set in OMAxisFilmOptions for the given axis).
  1.0 is maximum, 0.01 is 1%, and so forth.

  @param p_pct
  Percentage of speed to use as top speed, limited to 100%
  */

void SpeedControlProxy::maxSpeed(float p_pct) {
    if( p_pct > 1.0 )
        p_pct = 1.0;

    m_maxpct = p_pct;
}


void SpeedControlProxy::setDamping(float p_secs) {

    m_dampPeriods = (unsigned int) ( (1000.0 * p_secs) / (float) SCP_TIME_PERIOD );

    if( p_secs == 0.0 )
        m_dampPeriods = 1;

}


void SpeedControlProxy::speedPosChange(int p_value) {
    if( m_devSelected == false )
        return;

    qDebug() << "SCP: Received speed value: " << p_value << QThread::currentThreadId();

        // start timer (it will stop its self, if need be)
    startDampTimer();

        // change direction, if needed

    if( p_value < 0 ) {
            // bake in 5 points as a zero "dead zone"
        if( p_value > -10 ) {
            p_value = 0;
        }
        else {
            _changeDir(false);
            p_value += 10;
        }
    }
    else if( p_value > 0 ) {
        // bake in 5 points as a zero "dead zone"
        if( p_value < 10 ) {
            p_value = 0;
        }
        else {
            _changeDir(true);
            p_value -= 10;
        }
    }

    m_nextSpd = ((float) m_opts->maxSteps * m_maxpct) * ((float) abs(p_value) / 200.0);
        // reset damping ramp
    m_curPeriod = 1;

    qDebug() << "SCP: Got Speed request: " << m_nextSpd << "Steps/Sec";
}

 // handle the timer going off - make a speed change, if necessary
void SpeedControlProxy::_timerFire() {

        // do nothing if already at target speed
    if( m_nextSpd == m_curSpd )
        return;

    qDebug() << "SCP: DAMP: " << m_nextSpd << m_curSpd << QThread::currentThreadId();

        // determine if accelerating, or decelerating,
        // and difference between current speed and target
        // speed.

    float diff;
    bool rising = false;

    if( m_nextSpd > m_curSpd ) {
        rising = true;
        diff = m_nextSpd - m_curSpd;
    }
    else {
        diff = m_curSpd - m_nextSpd;
    }

    float spdChange = (diff / m_dampPeriods) * m_curPeriod;
    float newSpd    = rising ? m_curSpd + spdChange : m_curSpd - spdChange;

    if( newSpd < 0.05 || newSpd == 0.0 ) {
            // next target is 'stopped'
        qDebug() << "SCP: Sending Stop";

        m_curDev->stopMotor();

        m_curPeriod = m_dampPeriods;
        m_curSpd    = 0.0;

        stopDampTimer();
        emit motorStopped(m_curAddr);

        return;
    }
    else {

        if( m_curSpd == 0.0 ) {
                // we are currently stopped, so make sure
                // we get everything moving first
            setResolution(m_curRes);
            m_curDev->motorEnable();
            m_curDev->continuous(true);
                // set the speed before starting a move, so that we don't
                // start at some random speed for a brief moment
            m_curDev->speed(newSpd);
            m_curDev->move(m_curDir, 0);
            emit motorStarted(m_curAddr);
        }
        else {
                // not currently stopped, just change the speed
            m_curDev->speed(newSpd);
        }
    }

    m_curSpd = newSpd;
    m_curPeriod++;
}

void SpeedControlProxy::_changeDir(bool p_dir) {

    if( m_curDir == p_dir )
        return;

    qDebug() << "SCP: Changing direction to" << p_dir;

    m_curDir = p_dir;
    m_curDev->direction(m_curDir);

}

void SpeedControlProxy::deviceAdded(OMdeviceInfo *p_dev) {
    qDebug() << "SCP: Received new device" << p_dev->name;

    if( p_dev->device->type() == "nanoMoCo") {
        qDebug() << "SCP: Is nanoMoCo device.";
        m_devList.insert(p_dev->device->address(), p_dev);
    }
    else {
        qDebug() << "SCP: Is NOT nanoMoCo device. Ignoring.";
    }

}

void SpeedControlProxy::deviceRemoved(QString, unsigned short p_addr) {
    if( m_devList.contains(p_addr) ) {
        m_devList.remove(p_addr);
    }
}

void SpeedControlProxy::deviceChange(unsigned short p_addr) {

    qDebug() << "SCP: Change Device" << p_addr;

    if( m_nextSpd != m_curSpd ) {
        // oh, hold on a minute there buddy - can't change motors when still
        // moving to a speed!  This would suck if your first motor was damping
        // into a stop when you selected the second. (hint: the 2nd would start
        // moving and the first wouldn't stop)
        // we could be totally awesome and do BOTH, but for now - we just say
        // no way jose. Prevent the change from occuring until the move is complete

        qDebug() << "SCP: Still in movement, change denied";
        emit motorNotReady(m_curDev->address());
        return;
    }

    if( m_devList.contains(p_addr) ) {

        qDebug() << "SCP: Device is known to us, selecting";

        m_curDev      = static_cast<OMAxis*>(m_devList.value(p_addr)->device);
        m_opts        = m_optObj->getOptions(p_addr);
        m_devSelected = true;
        m_curAddr     = p_addr;

        qDebug() << "SCP: Emiting Acceptance of" << p_addr;
        emit motorChangeAccepted(p_addr);
        return;
    }
    else {
        qDebug() << "SCP: Device is unknown to us, ignoring";
    }

}


void SpeedControlProxy::setResolution(unsigned int p_ms) {

    if( m_devSelected == false )
        return;

    qDebug() << "SCP: Got resolution change: " << p_ms;

    if( p_ms != 1 && p_ms != 2 && p_ms != 4 && p_ms != 8 && p_ms != 16 ) {
        qDebug() << "SCP: Invalid MS value: " << p_ms;
        return;
    }

    m_curDev->microSteps(p_ms);
    m_curRes = p_ms;

}

float SpeedControlProxy::curSpeed() {
    return m_curSpd;
}
