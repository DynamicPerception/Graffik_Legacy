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

#ifndef MOTIONAREA_H
#define MOTIONAREA_H

#include <QWidget>
#include <QFrame>
#include <QPainter>
#include <QPaintEvent>
#include <QRect>
#include <QList>
#include <QTimer>
#include <QToolTip>
#include <QPoint>
#include <QPen>
#include <QString>
#include <QLinearGradient>


#include "MoCoBus/omnetwork.h"

#include "core/Options/axisoptions.h"
#include "core/Options/globaloptions.h"
#include "core/Utilities/timeconverter.h"
#include "core/Themer/singlethemer.h"

#include "film/FilmParameters/filmparameters.h"

#include "motionpathpainter.h"


const QString MA_BG_COLOR = "#5999BC";
const QString MA_CL_COLOR = "#FFFFFF";
const QString MA_ER_COLOR = "#FF3333";
const QString MA_MT_COLOR = "#CDC5BF";

const QString MA_STR_STEP = " Steps";
const QString MA_STR_IMP  = " In.";
const QString MA_STR_MET  = " cm.";
const QString MA_STR_DEG  = " Deg.";
const QString MA_STR_POS  = "Postion: ";
const QString MA_STR_SPD  = "Speed: ";
const QString MA_STR_MOD  = "/sec";
const QString MA_STR_TIM  = "Time: ";

const int MA_PT_NONE  = 0;
const int MA_PT_START = 1;
const int MA_PT_END   = 2;
const int MA_PT_ACE   = 3;
const int MA_PT_DCS   = 4;

const int MA_TT_TIMER = 500;

const int MA_MUTE_NA  = 0;
const int MA_MUTE_MT  = 1;
const int MA_MUTE_ER  = 2;

// grab handle size, in px
const unsigned int MA_GRAB_SIZE = 10;
// region growth around grab circle for registering a hit
const unsigned int MA_HIT_AREA = 40;

namespace Ui {
class MotionArea;
}


/** Motion Area Widget

  The Motion Area Widget is a widget which draws the motion path for
  a particular axis.

  @author
  C. A. Church
  */

class MotionArea : public QFrame {
    Q_OBJECT
    Q_PROPERTY(int lineWidth READ lineWidth WRITE setLineWidth DESIGNABLE true)
    Q_PROPERTY(QColor lineColor READ lineColor WRITE setLineColor DESIGNABLE true)
    Q_PROPERTY(QColor fillColorStart READ fillColorStart WRITE setFillColorStart DESIGNABLE true)
    Q_PROPERTY(QColor fillColorEnd READ fillColorEnd WRITE setFillColorEnd DESIGNABLE true)
    Q_PROPERTY(QColor grabLineColor READ grabLineColor WRITE setGrabLineColor DESIGNABLE true)
    Q_PROPERTY(QColor grabFillColor READ grabFillColor WRITE setGrabFillColor DESIGNABLE true)
    Q_PROPERTY(int muted READ muted DESIGNABLE true)

public:
    MotionArea(FilmParameters* c_film, OMdeviceInfo* c_dev, AxisOptions* c_aopt, GlobalOptions* c_gopt, QWidget *parent);
    ~MotionArea();

    void paintEvent(QPaintEvent* e);

    void mousePressEvent(QMouseEvent* p_event);
    void mouseMoveEvent(QMouseEvent* p_event);
    void mouseReleaseEvent(QMouseEvent* p_event);

    MotionPathPainter* getPathPainter();
    QList<QString> convertValue(float p_val);



    int lineWidth();
    void setLineWidth(int p_width);

    QColor lineColor();
    void setLineColor(QColor p_color);

    QColor fillColorStart();
    void setFillColorStart(QColor p_color);

    QColor fillColorEnd();
    void setFillColorEnd(QColor p_grad);

    QColor grabLineColor();
    void setGrabLineColor(QColor p_grad);

    QColor grabFillColor();
    void setGrabFillColor(QColor p_grad);

    int muted();

public slots:
    void filmUpdated();
    void scaleChange();
    void axisOptionsUpdated(OMaxisOptions* p_opts,unsigned short p_addr);
    void moveSane(bool p_sane);
    void mute();
    void playStatus(bool p_stat);

signals:
        /** Signal indicates when the display scale (relative or proportional) has been changed */
    void scaleChanged(bool p_scale);
        /** Signal indicates when the track has been muted */
    void muted(int p_muted);
        /** Signal indicates the global far-left and far-right x-pixel of the area (for aligning other elements) */
    void globalPosition(int p_left, int p_right);

private slots:
    void _tooltipTimer();

private:
    Ui::MotionArea *ui;
    OMdeviceInfo* m_dev;
    FilmParameters* m_film;
    MotionPathPainter* m_path;
    AxisOptions* m_aopt;
    GlobalOptions* m_gopt;
    QTimer* m_timer;

    QRect m_mvStart;
    QRect m_mvEnd;
    QRect m_acEnd;
    QRect m_dcStart;

    QColor m_lineColor;
    QColor m_fillStartColor;
    QColor m_fillEndColor;
    QColor m_grabLineColor;
    QColor m_grabFillColor;

    int m_lineWidth;

    int m_moveItem;
    int m_curPx;
    int m_curPy;
    int m_mute;
    bool m_sane;
    bool m_pstat;


    void _refreshStylesheet();
    bool _inHitRegion(QPoint* p_point, QRect p_rect);
};

#endif // MOTIONAREA_H
