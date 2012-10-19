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

#ifndef MOTIONPATHPAINTER_H
#define MOTIONPATHPAINTER_H

#include <QPainterPath>
#include <QRect>

#include "film/FilmParameters/filmparameters.h"
#include "core/Options/axisoptions.h"

    // rough and dirty travel constants from
    // OMMotor (on AVR)

#define MPP_TRAVEL_LIN      2.0
#define MPP_TRAVEL_QUAD     2.9999985
#define MPP_TRAVEL_QUADINV  1.5000597

#define MPP_DEF_MAX_PTS     2500


/** Motion Path Painer

  The Motion Path Painter does the heavy lifting of creating a painter path representing the
  travel of an axis, and provides key working methods for querying information about that
  path used in the operation of a Motion Area widget

  @author
  C. A. Church
  */

class MotionPathPainter : public QWidget
{
    Q_OBJECT
public:
    MotionPathPainter(unsigned short c_addr, FilmParameters* c_parms, AxisOptions* c_aopt, QWidget* parent);
    ~MotionPathPainter();
    
    void setMotionCurve();

    QPainterPath* getPath(QRect p_area);

    void maxSplinePoints(unsigned long p_points);
    unsigned long maxSplinePoints();

    int getStartPx();
    int getEndPx();
    int getAcEndPx();
    int getDcStartPx();
    int getMaxHeight();

    void scaling(bool p_scale);
    bool scaling();

    bool isDrawn();
    bool hasChanged();

    unsigned long getFilmTime(int p_x);
    float getSpeed(unsigned long p_x);
    float getPosition(unsigned long p_x);
    int getX(int p_time);

signals:
    
    /** Is the current motion profile sane?

      This signal is emitted when the motion profile is changed.  Sanity is defined
      as: the cruising speed does not exceed the maximum speed of the motor, and no
      other unlikely to succeed combination of parameters has been detected.

      @param p_state
      Movement state: true = sane, false = not sane
      */
    void moveSane(bool p_stat);

public slots:
    void paramsChanged();
    
private:

    struct s_splineCal {
        float acTm;
        float dcTm;
        float crTm;
        float topSpeed;
        float dcStart;
        float travel;
    };

    s_splineCal m_splinePlanned;

    FilmParameters* m_film;
    OMfilmAxisParams* m_axis;
    AxisOptions* m_aopt;

    unsigned short m_addr;

    QPainterPath* m_path;
    QRect m_area;

    int m_wasWidth;
    int m_wasHeight;

    unsigned long m_wasStart;
    unsigned long m_wasEnd;
    unsigned long m_wasDist;
    unsigned long m_wasAcc;
    unsigned long m_wasDcc;
    unsigned long m_wasMax;
    int m_wasEase;
    unsigned long m_wasLength;

    QList<float> m_renderPoints;
    QList<float> m_stepsTaken;

    unsigned long m_maxPoints;
    bool m_new;
    bool m_relativeScale;
    bool m_curveAvail;
    bool m_hasChanged;

    int m_startPx;
    int m_endPx;
    int m_dcStartPx;
    int m_acEndPx;
    int m_maxHeight;

    void _initSpline(unsigned long p_Steps, unsigned long p_Time, unsigned long p_Accel, unsigned long p_Decel, unsigned long p_totalSplines);
    float _qInvCalc(float p_tmPos);
    float _qEaseCalc(float p_tmPos);
    float _linearEasing(float p_tmPos);

    float _getScale();
    unsigned long _getMaxPoints();

};

#endif // MOTIONPATHPAINTER_H
