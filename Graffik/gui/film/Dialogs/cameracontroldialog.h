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

#ifndef CAMERACONTROLDIALOG_H
#define CAMERACONTROLDIALOG_H

#include <QDialog>
#include <QHash>

#include "core/Utilities/timeconverter.h"
#include "core/Themer/singlethemer.h"
#include "core/Options/axisoptions.h"
#include "MoCoBus/omnetwork.h"

#include "film/FilmParameters/filmparameters.h"

namespace Ui {
class CameraControlDialog;
}

/** Camera Controls Dialog Class

  This dialog displays controls available for the camera.

  @author
  C. A. Church
  */

class CameraControlDialog : public QDialog
{
    Q_OBJECT
    
public:
    CameraControlDialog(FilmParameters* c_params, OMNetwork* c_net, AxisOptions* c_opts, QWidget *parent = 0);
    ~CameraControlDialog();

signals:
    void intervalControlChanged(bool p_state);
    void fpsChanged(unsigned short p_fps);

public slots:
    void on_bulbExpToggle_selected(int p_val);
    void on_filmLenToggle_selected(int p_val);
    void on_manIntToggle_selected(int p_val);
    void on_focusConToggle_selected(int p_val);

    //void on_intervalCheck_stateChanged(int p_state);
    // void on_focusCheck_stateChanged(int p_state);
    //void on_autoCheck_stateChanged(int p_state);

    void accept();

private:
    Ui::CameraControlDialog *ui;

    FilmParameters* m_params;
    OMNetwork* m_net;
    AxisOptions* m_opts;

    int m_wasMaster;

    void _setupInputs();
    void _showManInterval(bool p_en);
};

#endif // CAMERACONTROLDIALOG_H
