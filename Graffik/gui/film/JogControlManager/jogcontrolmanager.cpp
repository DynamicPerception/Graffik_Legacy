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

JogControlManager::JogControlManager(OMNetwork* c_net, AxisOptions* c_opts, LiveDeviceModel* c_ldm, QDial* c_jogDial, QSlider* c_jogSpd, QSlider *c_jogDmp, QPushButton* c_homeBut, QPushButton* c_endBut, QObject *parent) :
    QObject(parent)
{
    m_curAxis = 0;
    m_curRes = 1;

  //  m_jogCombo = c_jogCombo;
    m_jogSpd = c_jogSpd;
    m_jogDmp = c_jogDmp;
    m_jogDial = c_jogDial;
    m_homeBut = c_homeBut;
    m_endBut = c_endBut;

    m_net = c_net;
    m_opts = c_opts;
    m_ldm = c_ldm;

    m_wantId = 0;
    m_wantType = 0;

        // populate resolution combo

 /*   m_jogCombo->addItem("Rapid", OM_JOGRES_RAPID);
    m_jogCombo->addItem("Coarse", OM_JOGRES_COARSE);
    m_jogCombo->addItem("Fine", OM_JOGRES_FINE);
    m_jogCombo->addItem("X Fine", OM_JOGRES_XFINE);
    m_jogCombo->setCurrentIndex(0);
*/
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


        // listen to jog spinners
    connect(m_jogSpd, SIGNAL(valueChanged(int)), this, SLOT(_jogMaxSpeedChange(int)));
    connect(m_jogDmp, SIGNAL(valueChanged(int)), this, SLOT(_jogDampChange(int)));

        // tie the jog dial into the speed control proxy
    connect(m_jogDial, SIGNAL(sliderMoved(int)), m_scp, SLOT(speedPosChange(int)));

        // tie resolution change to us (we'll pass onto SCP)
   // connect(m_jogCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(_jogResChange(int)));

        // tie home and end buttons
    connect(m_homeBut, SIGNAL(clicked()), this, SLOT(_homeClicked()));
    connect(m_endBut, SIGNAL(clicked()), this, SLOT(_endClicked()));

}

JogControlManager::~JogControlManager() {

    delete m_scp;
}

void JogControlManager::emergencyStop() {
    if( m_scp->curSpeed() != 0.0 )
        m_scp->speedPosChange(0);

    m_jogDial->setValue(0);
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
    _prepJogInputs(p_addr);

        // revert to rapid
   // m_jogCombo->setCurrentIndex(0);
    m_scp->setResolution(1);

}

double JogControlManager::_stepsToJogSpeed(OMaxisOptions* p_opts, unsigned int p_steps) {
    float setMove = 360.0;

    if( p_opts->axisType != AXIS_MOVE_ROT )
        setMove = 1.0;

    double spd = ( ((setMove / p_opts->ratio ) * (float) p_steps) * 60.0 ) / m_curRes;

    return( spd );

}

unsigned int JogControlManager::_jogSpeedToSteps(OMaxisOptions *p_opts, double p_speed) {
    float setMove = 360.0;

    if( p_opts->axisType != AXIS_MOVE_ROT )
        setMove = 1.0;

    return( ((p_speed / 60.0) / (setMove / p_opts->ratio)) * m_curRes );
}

void JogControlManager::_prepJogInputs(unsigned short p_addr) {

        // update the jog speed limit and damping with the saved
        // values for the axis
    OMaxisOptions* opts = m_opts->getOptions(p_addr);

    unsigned int jog_limit = opts->jogLimit;
    unsigned int   max_jog = opts->maxSteps;

        // TODO: Add indicator for displaying max speed and damping

    float curMax  = _stepsToJogSpeed(opts, jog_limit);
    float dispMax = _stepsToJogSpeed(opts, max_jog);

    qDebug() << "JCM: Setting current speed max value to" << curMax;

    m_jogSpd->setMinimum(1);
    m_jogSpd->setMaximum(dispMax);
    m_jogSpd->setValue(curMax);

    _jogMaxSpeedChange(curMax);

    m_jogDmp->setMinimum(1);
    m_jogDmp->setMaximum(30);
    m_jogDmp->setValue((int) opts->jogDamp);

    m_curAxis = p_addr;

}

void JogControlManager::_jogMaxSpeedChange(int p_spd) {
    if( m_curAxis == 0 )
        return;

    qDebug() << "JCM: Got max jog speed limit change to " << p_spd;

    OMaxisOptions* opts = m_opts->getOptions(m_curAxis);

    unsigned int steps = _jogSpeedToSteps(opts, p_spd);

    double spdPct = (double) steps / (double) opts->maxSteps;

    qDebug() << "JCM: Steps =" << steps << "Percentage:" << spdPct;

    m_scp->maxSpeed(spdPct);

        // record and save new value
    opts->jogLimit = steps;
    m_opts->setOptions(m_curAxis, opts);

}

void JogControlManager::_jogDampChange(int p_damp) {
    if( m_curAxis == 0 )
        return;

    qDebug() << "FW: Got jog damp change to " << p_damp;

    m_scp->setDamping(p_damp);

        // save damping value for axis
    OMaxisOptions* opts = m_opts->getOptions(m_curAxis);
    opts->jogDamp = p_damp;
    m_opts->setOptions(m_curAxis, opts);
}

// Handle changes to resolution combobox
// we do this here so that the SCP doesn't need to know
// anything about the combobox its self

void JogControlManager::_jogResChange(int p_idx) {

    // Temporarily disabled resolution changes from jog window
    // perhaps to be restored in the future, keeping code for now

    Q_UNUSED(p_idx);

   // unsigned int ms = m_jogCombo->itemData(p_idx).toUInt();
    unsigned int ms = 1; //m_jogCombo->itemData(p_idx).toUInt();

    m_scp->setResolution(ms);

    OMaxisOptions* opts = m_opts->getOptions(m_curAxis);

    m_curRes = ms;

        // update max jog speed display

    double newDisp = _stepsToJogSpeed(opts, opts->jogLimit);
    double dispMax = _stepsToJogSpeed(opts, opts->maxSteps);

    m_jogSpd->setMaximum(dispMax);
    m_jogSpd->setValue(newDisp);

}


void JogControlManager::_homeClicked() {
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

void JogControlManager::_endClicked() {
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
