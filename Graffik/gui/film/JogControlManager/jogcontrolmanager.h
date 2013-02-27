/*

  Graffik Motion Control Application

  Copyright (c) 2011-2013 Dynamic Perception

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

#ifndef JOGCONTROLMANAGER_H
#define JOGCONTROLMANAGER_H

#include <QObject>
#include <QDoubleSpinBox>
#include <QDial>
#include <QComboBox>
#include <QPushButton>
#include <QSlider>

#include "speedcontrolproxy.h"
#include "livedevicemodel.h"

#include "MoCoBus/omnetwork.h"
#include "core/Options/axisoptions.h"


const int OM_JOGRES_RAPID   = 1;
const int OM_JOGRES_COARSE  = 2;
const int OM_JOGRES_FINE    = 8;
const int OM_JOGRES_XFINE   = 16;

/** Jog Controls Manager

  This class manages the interactions with the Jog Controls on the
  film screen.

  @author
  C. A. Church
  */

class JogControlManager : public QObject
{
    Q_OBJECT
public:
    JogControlManager(OMNetwork* c_net, AxisOptions* c_opts, LiveDeviceModel* c_ldm, QDial* c_jogDial, QSlider* c_jogSpd, QSlider* c_jogDmp, QPushButton* c_homeBut, QPushButton* c_endBut, QObject *parent = 0);
    ~JogControlManager();

signals:
        // if the SCP denies a chaneg to motor (usually because a damp is still in progress)
        // this signal will be emitted, indicating which motor is -still- selected
    void motorChangeDenied(unsigned short p_keepaddr);
        // end position determined via button click
    void endPosition(unsigned short p_addr, long p_end);

public slots:
    void playStatusChange(bool p_stat);
    void emergencyStop();

private slots:
    void _liveDeviceSelected(unsigned short p_addr);
    void _jogMaxSpeedChange(int p_spd);
    void _jogDampChange(int p_damp);
    void _jogResChange(int p_idx);
    void _homeClicked();
    void _endClicked();

    void _cmdComplete(int p_id, OMCommandBuffer* p_cmd);

private:
    unsigned short m_curAxis;
    unsigned int m_curRes;

    QSlider* m_jogSpd;
    QSlider* m_jogDmp;
    QDial* m_jogDial;
    QComboBox* m_jogCombo;
    QPushButton* m_homeBut;
    QPushButton* m_endBut;

    OMNetwork* m_net;
    AxisOptions* m_opts;
    LiveDeviceModel* m_ldm;
    SpeedControlProxy* m_scp;

    int m_wantId;
    int m_wantType;

    static const int s_typeEnd = 1;

    double _stepsToJogSpeed(OMaxisOptions* p_opts, unsigned int p_steps);
    unsigned int _jogSpeedToSteps(OMaxisOptions* p_opts, double p_speed);
    void _prepJogInputs(unsigned short p_addr);

};

#endif // JOGCONTROLMANAGER_H
