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

#include "jogcontrolmanager.h"

#include <QtEndian>

#include <QDebug>

JogControlManager::JogControlManager(OMNetwork* c_net, AxisOptions* c_opts, LiveDeviceModel* c_ldm, QObject *parent) : QObject(parent) {
    m_curAxis  = 0;
    m_curRes   = 1;
    m_net      = c_net;
    m_opts     = c_opts;
    m_ldm      = c_ldm;
    m_wantId   = 0;
    m_wantType = 0;


        // create speedcontrolproxy object
    m_scp = new SpeedControlProxy(m_opts);

        // ensure the SCP knows when a new device has been added
    connect(m_net, SIGNAL(deviceAdded(OMdeviceInfo*)), m_scp, SLOT(deviceAdded(OMdeviceInfo*)));
    connect(m_net, SIGNAL(deviceDeleted(QString,unsigned short)), m_scp, SLOT(deviceRemoved(QString,unsigned short)));

        // we need to listen to some commands we issue
    connect(m_net, SIGNAL(complete(int,OMCommandBuffer*)), this, SLOT(_cmdComplete(int,OMCommandBuffer*)), Qt::QueuedConnection);

        // inform SCP of a new device selected for jog control (via livedevicemodel)
    connect(m_ldm, SIGNAL(deviceSelected(unsigned short)), m_scp, SLOT(deviceChange(unsigned short)));

        // of course, SCP may not allow the change yet (still moving to target speed)
        // so we'll need to pass this signal up as needed

    connect(m_scp, SIGNAL(motorNotReady(unsigned short)), this, SIGNAL(motorChangeDenied(unsigned short)));

        // we need to know when a device is changed so that we can modify the jogspeed and jogdamp spin boxes as needed
    connect(m_scp, SIGNAL(motorChangeAccepted(unsigned short)), this, SLOT(_liveDeviceSelected(unsigned short)));
    connect(m_scp, SIGNAL(motorChangeAccepted(unsigned short)), this, SIGNAL(motorChangeAllowed(unsigned short)));

        // reflect signals out from SCP
    connect(m_scp, SIGNAL(motorStarted(unsigned short)), this, SIGNAL(motorStarted(unsigned short)));
    connect(m_scp, SIGNAL(motorStopped(unsigned short)), this, SIGNAL(motorStopped(unsigned short)));

}

JogControlManager::~JogControlManager() {

    delete m_scp;
}

void JogControlManager::speedChange(int p_spd) {
    m_scp->speedPosChange(p_spd);
}

void JogControlManager::emergencyStop() {
    if( m_scp->curSpeed() != 0.0 )
        m_scp->speedPosChange(0);

}

void JogControlManager::playStatusChange(bool p_stat) {
    Q_UNUSED(p_stat);
        // any change to play status should immediately stop the
        // speed control proxy

    if( m_scp->curSpeed() != 0.0 )
        m_scp->speedPosChange(0);
}

void JogControlManager::_liveDeviceSelected(unsigned short p_addr) {

    qDebug() << "JCM: Received selection for device addr" << p_addr;

    m_curAxis = p_addr;
            // revert to rapid
    m_scp->setResolution(1);

        // make sure sleep enable is set properly for axis

    OMdeviceInfo* devInfo = m_net->getDevices().value(m_curAxis);
    OMDevice*         dev = devInfo->device;
    OMAxis*        omaxis = dynamic_cast<OMAxis*>(dev);
    OMaxisOptions*   opts = m_opts->getOptions(p_addr);

    if( omaxis == 0 ) {
        qDebug() << "JCM: Error Casting Device!";
        return;
    }

    omaxis->sleep(opts->sleep);

}

/** Convert Steps per Second to Speed

  Given a pointer to an OMaxisOptions instance, a given steps per second, and a current resolution,
  returns a native speed for the motor in distance per minute.
  */

double JogControlManager::stepsToJogSpeed(OMaxisOptions* p_opts, unsigned int p_steps, int p_res) {
    float setMove = 360.0;

    if( p_opts->axisType != AXIS_MOVE_ROT )
        setMove = 1.0;

    double spd = ( ((setMove / p_opts->ratio ) * (float) p_steps) * 60.0 ) / p_res;

    return( spd );

}

/** Convert Speed to Steps per Second

  Given a pointer an OMaxisOptions instance, a given speed (in native units for the axis), and a current resolution,
  returns a speed in steps per second.
  */

unsigned int JogControlManager::jogSpeedToSteps(OMaxisOptions *p_opts, double p_speed, int p_res) {
    float setMove = 360.0;

    if( p_opts->axisType != AXIS_MOVE_ROT )
        setMove = 1.0;

    return( ((p_speed / 60.0) / (setMove / p_opts->ratio)) * p_res );
}


/** Jog Max Speed Changed Slot

  Set the maximum jog speed

  Does nothing if no axis is selected.

  */

void JogControlManager::jogMaxSpeedChange(int p_spd) {
    if( m_curAxis == 0 )
        return;

    qDebug() << "JCM: Got max jog speed limit change to " << p_spd;

    OMaxisOptions* opts = m_opts->getOptions(m_curAxis);

    unsigned int steps = jogSpeedToSteps(opts, p_spd, m_curRes);

    double spdPct = (double) steps / (double) opts->maxSteps;

    qDebug() << "JCM: Steps =" << steps << "Percentage:" << spdPct;

    m_scp->maxSpeed(spdPct);

        // record and save new value
    opts->jogLimit = steps;
    m_opts->setOptions(m_curAxis, opts);

}

/** Jog Damping Value Changed

  Set jog damping amount, in seconds.

  Does nothing if no axis is selected
  */

void JogControlManager::jogDampChange(int p_damp) {
    if( m_curAxis == 0 )
        return;

    qDebug() << "FW: Got jog damp change to " << p_damp;

    m_scp->setDamping(p_damp);

        // save damping value for axis
    OMaxisOptions* opts = m_opts->getOptions(m_curAxis);
    opts->jogDamp = p_damp;
    m_opts->setOptions(m_curAxis, opts);
}


/** Change Move Resolution Slot */

void JogControlManager::jogResChange(int p_ms) {

    m_scp->setResolution(p_ms);
    m_curRes = p_ms;

        // update max jog speed display - TODO: this needs to be
        // moved upstream, to the JogControlPanel, saving for that move
        // later, if code ever needed.

    /* OMaxisOptions* opts = m_opts->getOptions(m_curAxis);

    unsigned int ms = m_jogCombo->itemData(p_idx).toUInt();

    double newDisp = stepsToJogSpeed(opts, opts->jogLimit, m_curRes);
    double dispMax = stepsToJogSpeed(opts, opts->maxSteps, m_curRes);

    m_jogSpd->setMaximum(dispMax);
    m_jogSpd->setValue(newDisp);*/

}


/** Set Current Axis Position as Home */

void JogControlManager::setHome() {
    if( m_curAxis == 0 )
        return;

    OMdeviceInfo* devInfo = m_net->getDevices().value(m_curAxis);
    OMDevice* dev = devInfo->device;

    OMAxis* omaxis = dynamic_cast<OMAxis*>(dev);

    if( omaxis == 0 ) {
        qDebug() << "JCM: Error Casting Device!";
        return;
    }

    qDebug() << "JCM: Sending SetHome";

        // TODO: Validate That this succeeds!
    omaxis->setHome();
}

/** Set Current Axis Position as End */

void JogControlManager::setEnd() {
    if( m_curAxis == 0 )
        return;

    OMdeviceInfo* devInfo = m_net->getDevices().value(m_curAxis);
    OMDevice* dev = devInfo->device;

    OMAxis* omaxis = dynamic_cast<OMAxis*>(dev);

    if( omaxis == 0 ) {
        qDebug() << "JCM: Error Casting Device!";
        return;
    }

    qDebug() << "JCM: Requesting Current Position";

    int cmdId = omaxis->getHomeDist();
    m_net->getManager()->hold(cmdId);

    m_wantId = cmdId;
    m_wantType = s_typeEnd;
}


 // This slot handles reading the results of commands we need to access
void JogControlManager::_cmdComplete(int p_id, OMCommandBuffer *p_cmd) {

    if( m_wantId == 0 || m_wantId != p_id )
        return;

    long distance = 0;
    bool   doEmit = false;

    if( m_wantType == s_typeEnd ) {
        if( p_cmd->status() != OMC_SUCCESS ) {
            qDebug() << "JCM: ERROR! Did Not Get Success for cmd id" << p_id << "Got" << p_cmd->status();
            m_net->getManager()->release(p_id);
            return;
        }
        else {
            unsigned int resSize = p_cmd->resultSize();

            if( resSize > 0 ) {

                char* res = new char[resSize];
                p_cmd->result(res, resSize);

                distance = qFromBigEndian<qint32>((uchar*)res);
                delete res;

                doEmit = true;

                qDebug() << "JCM: Got End Position" << QString::number(distance);

            }
        }

    }

    m_wantId = 0;
    m_wantType = 0;

    m_net->getManager()->release(p_id);

    if( doEmit )
        emit endPosition(m_curAxis, distance);

}

