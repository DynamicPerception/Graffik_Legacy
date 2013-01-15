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
