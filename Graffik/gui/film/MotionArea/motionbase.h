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

#ifndef MOTIONBASE_H
#define MOTIONBASE_H

#include <QWidget>

#include "motionarea.h"
#include "MoCoBus/omnetwork.h"
#include "core/Options/axisoptions.h"
#include "core/Options/globaloptions.h"
#include "core/Themer/singlethemer.h"
#include "film/FilmParameters/filmparameters.h"

#define MB_STR_POS  "Position:"

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
    
public slots:
    void curScale(bool p_scale);
    void muted(int p_muted);
    void on_easeButton_clicked();
    void on_resButton_clicked();

    void statusChange(bool p_stat);
    void currentPlayStatus(bool p_stat, unsigned long p_runTime);

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


};

#endif // MOTIONBASE_H
