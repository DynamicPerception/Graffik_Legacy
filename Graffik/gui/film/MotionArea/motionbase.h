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

#ifndef MOTIONBASE_H
#define MOTIONBASE_H

#include <QWidget>
#include <QString>

#include "motionarea.h"
#include "ErrorConsole/motionerrordialog.h"

#include "MoCoBus/omnetwork.h"
#include "core/Options/axisoptions.h"
#include "core/Options/globaloptions.h"
#include "core/Themer/singlethemer.h"
#include "core/Dialogs/confirmdialog.h"
#include "film/FilmParameters/filmparameters.h"
#include "film/Dialogs/trackinfodialog.h"

const QString MB_STR_POS = "Position:";
const QString MB_STR_ISM = "Camera Connected";
const QString MB_STR_NTM = "No Camera";
const QString MB_STR_DEL = "WARNING: Move Delete Cannot Be Undone, Continue?";

namespace Ui {
class MotionBase;
}

/** Motion Base Widget

  The Motion Base Widget provides information about the axis, and serves as a container widget for the Motion Area

  @author
  C. A. Church
  */

class MotionBase : public QWidget
{
    Q_OBJECT
    
public:
    MotionBase(FilmParameters* c_film, OMdeviceInfo* c_dev, AxisOptions* c_aopts, GlobalOptions* c_gopts, QWidget *parent);
    ~MotionBase();
    
    MotionArea* area();

public slots:
    void curScale(bool p_scale);
    void muted(int p_muted);
    void on_easeButton_clicked();
    void on_resButton_clicked();
    void on_nameButton_clicked();
    void on_errorButton_clicked();
    void on_masterButton_clicked();
    void on_delButton_clicked();

    void statusChange(bool p_stat);
    void currentPlayStatus(bool p_stat, unsigned long p_runTime);
    void deviceOptionsChanged(OMaxisOptions* p_opts, unsigned short p_addr);

private slots:
    void _themeChanged();

signals:

        /** Reflected signal from Motion Area, indicates the left X and right X borders
            (in global coordinates) of the area drawn */

    void areaBorders(int p_leftX, int p_rightX);

        /** Reflect Signal */
    void playStatus(bool p_stat);

private:
    Ui::MotionBase *ui;
    MotionArea* m_area;
    FilmParameters* m_film;
    OMdeviceInfo* m_dev;
    AxisOptions* m_opts;

    unsigned short m_addr;


};

#endif // MOTIONBASE_H
