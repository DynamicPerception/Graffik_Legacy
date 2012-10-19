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

#ifndef CAMERACONTROLDIALOG_H
#define CAMERACONTROLDIALOG_H

#include <QDialog>

#include "core/Utilities/timeconverter.h"

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
    CameraControlDialog(FilmParameters* c_params, QWidget *parent = 0);
    ~CameraControlDialog();

signals:
    void intervalControlChanged(bool p_state);
    void fpsChanged(unsigned short p_fps);

public slots:
    void on_bulbCheck_stateChanged(int p_state);
    void on_focusCheck_stateChanged(int p_state);
    void on_intervalCheck_stateChanged(int p_state);
    void on_autoCheck_stateChanged(int p_state);

    void accept();

private:
    Ui::CameraControlDialog *ui;

    FilmParameters* m_params;

    void _setupInputs();
};

#endif // CAMERACONTROLDIALOG_H
