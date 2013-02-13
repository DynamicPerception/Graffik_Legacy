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

#ifndef TRACKINFODIALOG_H
#define TRACKINFODIALOG_H

#include <QDialog>
#include <QString>

#include "core/Options/axisoptions.h"
#include "core/Utilities/timeconverter.h"
#include "film/FilmParameters/filmparameters.h"
#include "core/Themer/singlethemer.h"

const QString TID_STR_STARTM = "Move Start Time: ";
const QString TID_STR_ENDTM  = "Move End Time:   ";
const QString TID_STR_ACTM   = "Accel Length:    ";
const QString TID_STR_DCTM   = "Decel Length:    ";
const QString TID_STR_DIST   = "Travel Distance: ";
const QString TID_STR_EASE   = "Easing:          ";
const QString TID_STR_EASQ   = "Quadratic";
const QString TID_STR_EASI   = "Inverse Quadratic";
const QString TID_STR_EASL   = "Linear";
const QString TID_STR_FSTR   = "Film Start";
const QString TID_STR_FEND   = "Film End";

namespace Ui {
class TrackInfoDialog;
}

/** Track Info Dialog

  Displays information about the track's movement in a pop-up dialog

  @author
  C. A. Church
  */
class TrackInfoDialog : public QDialog {
    Q_OBJECT
    
public:
    TrackInfoDialog(OMfilmAxisParams* c_parms,  QWidget *parent = 0);
    ~TrackInfoDialog();
    
private:
    Ui::TrackInfoDialog *ui;
};

#endif // TRACKINFODIALOG_H
