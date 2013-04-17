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

#ifndef JOGCONTROLPANEL_H
#define JOGCONTROLPANEL_H

#include <QWidget>
#include <QString>

#include "jogcontrolmanager.h"
#include "livedevicemodel.h"

#include "core/Options/axisoptions.h"
#include "core/Options/globaloptions.h"
#include "core/Themer/singlethemer.h"

#include "film/FilmParameters/filmparameters.h"
#include "MoCoBus/omnetwork.h"


const int JCP_MAX_DAMP  = 30;

const QString JCP_STR_DAMP = "Damping: %1 Seconds";
const QString JCP_STR_SPD  = "Max Speed: %1 %2/Minute";
const QString JCP_STR_DEG  = "Deg.";
const QString JCP_STR_IN   = "In.";
const QString JCP_STR_CM   = "Cm.";
const QString JCP_STR_STEP = "Steps";

namespace Ui {
class JogControlPanel;
}

/** Jog Control Panel Widget Class

  This class creates and manages the plug-in panel widget for the Jog Control Manager

  @author
  C. A. Church
  */

class JogControlPanel : public QWidget
{
    Q_OBJECT
    
public:
    JogControlPanel(OMNetwork* c_net, AxisOptions* c_opts, GlobalOptions* c_gopts, FilmParameters* c_params, QWidget *parent = 0);
    ~JogControlPanel();
    
signals:

    void playStatusChange(bool p_stat);
    void emergencyStop();


private slots:
    void _endSet(unsigned short p_addr, long p_dist);
    void _homeSet();
    void _jogMotorChangeDenied(unsigned short p_oldAddr);
    void _themeChanged();
    void _jogMotorChangeAllowed(unsigned short p_addr);
    void _modeClicked();
    void _eStop();
    void _dialReleased();
    void _motorStarted(unsigned short p_addr);
    void _motorStopped(unsigned short p_addr);
    void _jogDampChanged(int p_val);
    void _jogSpeedChanged(int p_val);

private:
    Ui::JogControlPanel *ui;

    OMNetwork* m_net;
    AxisOptions* m_opts;
    GlobalOptions* m_gopts;
    FilmParameters* m_params;
    JogControlManager* m_jcm;
    LiveDeviceModel* m_ldModel;

    bool m_curMode;
    unsigned short m_curAddr;
    QHash<unsigned short, QList<bool> > m_ledStates;

    void _prepJogInputs(unsigned short p_addr);
};

#endif // JOGCONTROLPANEL_H
