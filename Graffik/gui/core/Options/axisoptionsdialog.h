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

#ifndef OMAXISFILMOPTIONSDIALOG_H
#define OMAXISFILMOPTIONSDIALOG_H

#include <QDialog>

#include "axisoptions.h"
#include "core/Widgets/slidetoggle.h"

#define AOD_STR_MAST    "By setting this device as the timing master, you will automatically unset any existing master device. Do you want to do this?"
#define AOD_STR_UNMAST  "If you unset this device as the timing master, you must manually select and edit another device and configure it as timing master for synchronization to work. Do you want to do this?"

#define AOD_STR_PAN     "VX1 Pan"
#define AOD_STR_TILT    "VX1 Tilt"
#define AOD_STR_SLIDE   "VX1 Slider"
#define AOD_STR_CUST    "Custom"

#define AOD_STR_ROT     "Rotary"
#define AOD_STR_LIN     "Linear"

namespace Ui {
class AxisOptionsDialog;
}

/** Axis Options Dialog Class

  This dialog allows the user to manage options for an axis node type

  @author
  C. A. Church
  */
class AxisOptionsDialog : public QDialog
{
    Q_OBJECT
    
public:
    AxisOptionsDialog(AxisOptions* c_opts, unsigned short c_addr, QWidget *parent = 0);
    ~AxisOptionsDialog();
    
public slots:
    void on_saveButton_clicked();
    void on_cancelButton_clicked();

private slots:
    void _defaultComboChange(int p_idx);

private:
    Ui::AxisOptionsDialog* ui;
    unsigned short m_addr;
    OMaxisOptions* m_opts;
    AxisOptions* m_optObj;

    void _initInputs();

};

#endif // OMAXISFILMOPTIONSDIALOG_H
