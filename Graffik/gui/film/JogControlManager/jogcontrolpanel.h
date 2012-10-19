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

#ifndef JOGCONTROLPANEL_H
#define JOGCONTROLPANEL_H

#include <QWidget>

#include "jogcontrolmanager.h"
#include "livedevicemodel.h"

#include "core/Options/axisoptions.h"
#include "film/FilmParameters/filmparameters.h"
#include "MoCoBus/omnetwork.h"

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
    JogControlPanel(OMNetwork* c_net, AxisOptions* c_opts, FilmParameters* c_params, QWidget *parent = 0);
    ~JogControlPanel();
    
private slots:
    void _endSet(unsigned short p_addr, long p_dist);
    void _jogMotorChangeDenied(unsigned short p_oldAddr);

private:
    Ui::JogControlPanel *ui;

    OMNetwork* m_net;
    AxisOptions* m_opts;
    FilmParameters* m_params;
    JogControlManager* m_jcm;
    LiveDeviceModel* m_ldModel;
};

#endif // JOGCONTROLPANEL_H
