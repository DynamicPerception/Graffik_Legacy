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

#include "motionarea.h"
#include "ui_motionarea.h"



#include <QDebug>


MotionArea::MotionArea(FilmParameters *c_film, OMdeviceInfo *c_dev, AxisOptions* c_aopt, GlobalOptions *c_gopt, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MotionArea)
{
    m_dev = c_dev;
    m_film = c_film;
    m_aopt = c_aopt;
    m_gopt = c_gopt;

    m_bgCol = MA_BG_COLOR;
    m_mute  = MA_MUTE_NA;
    m_sane  = true;

    m_pstat = false;

    ui->setupUi(this);

    ui->gridLayout->setContentsMargins(0,0,0,0);
    ui->gridLayout->setMargin(0);
    ui->gridLayout->setSpacing(0);

    m_path = new MotionPathPainter(c_dev->device->address(), m_film, m_aopt, this);
    m_path->setMotionCurve();
    m_path->scaling(false);

    m_moveItem = MA_PT_NONE;

    m_timer = new QTimer;
    m_timer->setSingleShot(true);

    connect(m_film, SIGNAL(paramsReleased()), this, SLOT(filmUpdated()));
    connect(m_aopt, SIGNAL(deviceOptionsChanged(OMaxisOptions*,unsigned short)), this, SLOT(axisOptionsUpdated(OMaxisOptions*,unsigned short)));
    connect(m_path, SIGNAL(moveSane(bool)), this, SLOT(moveSane(bool)));
    connect(m_timer, SIGNAL(timeout()), this, SLOT(_tooltipTimer()));

        // enable mouse tracking so we can draw grab points
    this->setMouseTracking(true);
}

MotionArea::~MotionArea()
{
    delete ui;
    delete m_path;

    m_timer->stop();
    delete m_timer;
}


 /** Return a pointer to the MotionPathPainter object

    No caller should retain this pointer after use.
 */

MotionPathPainter* MotionArea::getPathPainter() {
    return m_path;
}

void MotionArea::filmUpdated() {
    this->update();
}


void MotionArea::scaleChange() {
        // flip scale
    m_path->scaling(! m_path->scaling() );
    this->update();
    emit scaleChanged(m_path->scaling());
}

/** Slot for receiving changes to axis option */

void MotionArea::axisOptionsUpdated(OMaxisOptions*, unsigned short p_addr) {
    if( p_addr != m_dev->device->address() )
        return;

    qDebug() << "MA: Got Axis Options Change";
    this->update();
}

/** Handle Mouse Press inside of Widget */

void MotionArea::mousePressEvent(QMouseEvent *p_event) {
    QPoint cPos = p_event->pos();

        // stop tooltip display timer
    m_timer->stop();

        // note that we never allow click+drag when the film is playing,
        // so check the play status

    if( m_pstat == false && m_mvStart.contains(cPos) ) {
        qDebug() << "MA: Clicked MVStart";
        m_moveItem = MA_PT_START;
    }
    else if( m_pstat == false && m_mvEnd.contains(cPos) ) {
        qDebug() << "MA: Clicked MVEnd";
        m_moveItem = MA_PT_END;
    }
    else if( m_pstat == false && m_acEnd.contains(cPos) ) {
        qDebug() << "MA: Clicked ACEnd";
        m_moveItem = MA_PT_ACE;
    }
    else if(m_pstat == false && m_dcStart.contains(cPos) ) {
        qDebug() << "MA: Clicked DCStart";
        m_moveItem = MA_PT_DCS;
    }
    else {
        m_moveItem = MA_PT_NONE;
        // we're clicking the mouse inside of the area,
        // but we haven't clicked on a grab circle.  We
        // want to re-set our timer, which will look
        // for us hovering over one spot for MA_TT_TIMER period
        m_timer->stop();
        m_timer->start(MA_TT_TIMER);
        m_curPx = cPos.x();
        m_curPy = cPos.y();
    }


}

/** Handle Mouse Movement inside of Widget */

void MotionArea::mouseMoveEvent(QMouseEvent *p_event) {



    if( m_moveItem == MA_PT_NONE ) {
        this->update();
        return;
    }
    else {
            // turn off timer,
        m_timer->stop();
    }

    int newX = p_event->pos().x();

    if( m_moveItem == MA_PT_START ) {
        unsigned long startTm = m_path->getFilmTime(newX);
        unsigned long acTm = m_path->getFilmTime(m_path->getAcEndPx());
        unsigned long dcTm = m_path->getFilmTime(m_path->getDcStartPx());

        OMfilmParams* parms = m_film->getParams();
        OMfilmAxisParams* axis = parms->axes.value(m_dev->device->address());

            // sanity check!
        if( startTm > axis->startTm ) {
            if( acTm + (startTm - axis->startTm) > dcTm )
                startTm = axis->startTm;
        }

        axis->startTm = startTm;
        m_film->releaseParams();
    }
    else if( m_moveItem == MA_PT_END ) {
        unsigned long endTm = m_path->getFilmTime(newX);
        unsigned long acTm = m_path->getFilmTime(m_path->getAcEndPx());
        unsigned long dcTm = m_path->getFilmTime(m_path->getDcStartPx());

        OMfilmParams* parms = m_film->getParams();
        OMfilmAxisParams* axis = parms->axes.value(m_dev->device->address());


        if( endTm < axis->endTm ) {
            if( dcTm - (axis->endTm - endTm ) < acTm )
                endTm = axis->endTm;
        }

        qDebug() << "MA: ET" << endTm << dcTm << axis->decelTm;

        axis->endTm = endTm;
        m_film->releaseParams();
    }
    else if( m_moveItem == MA_PT_ACE ) {
        unsigned long acEnd = m_path->getFilmTime(newX);
        unsigned long oldAc = m_path->getFilmTime(m_path->getAcEndPx());

        unsigned long dcTm = m_path->getFilmTime(m_path->getDcStartPx());

        OMfilmParams* parms = m_film->getParams();
        OMfilmAxisParams* axis = parms->axes.value(m_dev->device->address());

        long diff = acEnd - oldAc;

            // limit move to decel start point
        if( acEnd > dcTm ) {
            acEnd = dcTm;
            diff = 0;
        }

        if( acEnd <= axis->startTm ) {
            acEnd = axis->startTm + 100;
            diff = 0;
        }



        qDebug() << "MA: AC: " << acEnd << oldAc << dcTm << diff << axis->startTm << (axis->accelTm + diff) ;

        axis->accelTm = axis->accelTm + diff;
        m_film->releaseParams();
    }
    else if( m_moveItem == MA_PT_DCS ) {
        unsigned long dcStart = m_path->getFilmTime(newX);
        unsigned long endTm   = m_path->getFilmTime(m_path->getEndPx());
        unsigned long oldDc   = m_path->getFilmTime(m_path->getDcStartPx());

        unsigned long acTm = m_path->getFilmTime(m_path->getAcEndPx());

        OMfilmParams* parms = m_film->getParams();
        OMfilmAxisParams* axis = parms->axes.value(m_dev->device->address());

        long diff = dcStart - oldDc;

            // limit move the ac end point
        if( dcStart < acTm ) {
            dcStart = acTm;
            diff = 0;
        }

        if( axis->endTm > 0 && dcStart >= axis->endTm ) {
            dcStart = axis->endTm - 100;
            diff = 0;
        }



        axis->decelTm = endTm - dcStart;

        qDebug() << "MA: DC: " << dcStart << endTm << axis->decelTm;

        m_film->releaseParams();
    }

  //  this->update();
}

/** Handle mouse Release Events inside of widget */

void MotionArea::mouseReleaseEvent(QMouseEvent *) {
    m_moveItem = MA_PT_NONE;
}

/** Override Paint Event - Draw Motion Path */

void MotionArea::paintEvent(QPaintEvent *e) {
    QPainter painter(this);
    QRect eventRect = e->rect();

   // painter.fillRect(eventRect, QColor(m_bgCol));
    painter.drawPath(*m_path->getPath(eventRect));

        // let everyone know where our left and right sides are, so tied elements can line up
    emit globalPosition(mapToGlobal(QPoint(eventRect.topLeft().x(), 0)).x(), mapToGlobal(QPoint(eventRect.topRight().x(), 0)).x());

        // Regenerate grab-and-drag circles if the path has changed, but only
        // do so if the film is not playing

        // TODO: Remove hard-coded sizes.

    if( m_pstat == false ) {
        if( m_path->hasChanged() ) {
            m_mvStart.setRect(m_path->getStartPx() - 5, eventRect.bottom() - 5, 10, 10);
            m_mvEnd.setRect(m_path->getEndPx() - 5, eventRect.bottom() - 5, 10, 10);
            m_acEnd.setRect(m_path->getAcEndPx() - 5, m_path->getMaxHeight() - 5, 10, 10);
            m_dcStart.setRect(m_path->getDcStartPx() - 5, m_path->getMaxHeight() - 5, 10, 10);
        }

         // draw drag points for start and end time
        if( m_path->isDrawn() && QWidget::underMouse() ) {
            painter.drawEllipse(m_mvStart);
            painter.drawEllipse(m_mvEnd);
            painter.drawEllipse(m_acEnd);
            painter.drawEllipse(m_dcStart);
        }
    }

}

/** Slot For Sane Movement Signal from MotionPathPainter */

void MotionArea::moveSane(bool p_sane) {

        // handle mute check for bg color
    if( p_sane ) {


            // do not change mute setting on sane check, unless it was
            // set to error by this function
        if( m_mute == MA_MUTE_ER  || m_mute == MA_MUTE_NA ) {
            m_mute = MA_MUTE_NA;
            m_bgCol = MA_BG_COLOR;
        }
        else {
                // leave muted if the mute wasn't triggered by error
            m_bgCol = MA_MT_COLOR;
        }

    }
    else {
        m_bgCol = MA_ER_COLOR;
            // do not change mute setting if track already muted
        if( m_mute != MA_MUTE_MT )
            m_mute = MA_MUTE_ER;

    }

        // inform other components that this track is either
        // muted, or un-muted.

    m_sane = p_sane;
        // careful of feedback loop - do not let any slot
        // attached to this attempt to broadcast a change to
        // film parameters
    emit muted(m_mute);

}

/** Convert a Given Speed or Distance Value

  Converts the given speed or distance value to the correct
  numeric representation, based on the user's preference of
  imperial, metric, or steps.

  @return
  A QList<QString> with two elements, the first being the string
  representation of the number (fixed to two decimal places), and the
  second being the textual label correct for the unit type.

  */

QList<QString> MotionArea::convertValue(float p_val) {
    OMaxisOptions* devOpts = m_aopt->getOptions(m_dev->device->address());

    qDebug() << "MA: cV: Got " << p_val;

    float devRatio = devOpts->ratio;

    int dispType = m_gopt->display();
    QString dLabel = MA_STR_STEP;
    bool forceZero = false;

        // FP division can do miraculous things...
    if( p_val < 0.0001 )
        forceZero = true;

    if( dispType != Options::Steps ) {
        if( devOpts->axisMove == AXIS_MOVE_ROT ) {
            p_val = p_val * (360.0 / devRatio);
            dLabel = MA_STR_DEG;
        }
        else {
            if( dispType == Options::Imperial ) {
                p_val = p_val * (1.0 / devRatio);
                dLabel = MA_STR_IMP;
            }
            else {
                p_val = p_val * (2.54 / devRatio);
                dLabel = MA_STR_MET;
            }

        }
    }

        // deal with very low FP values
    p_val = p_val < 0.0001 ? 0.0 : p_val;

        // if we started with nothing, let's end with nothing
    if( forceZero == true )
        p_val = 0.0;

    QList<QString> ret;
    ret.append(QString::number(p_val, 'f', 2));
    ret.append(dLabel);

    return ret;
}

/** Slot for when ToolTip Timer Triggers */

void MotionArea::_tooltipTimer() {
        // don't do anything if we're not under the
        // mouse any more!
    if( ! QWidget::underMouse() )
        return;

        // determine position to draw tooltip
    QPoint point = this->mapToGlobal(QPoint(m_curPx, m_curPy));

    float curPos = m_path->getPosition(m_curPx);
    float curSpd = m_path->getSpeed(m_curPx);
    unsigned long curMs = m_path->getFilmTime(m_curPx);

    QList<QString> posDisp = convertValue(curPos);
    QList<QString> spdDisp = convertValue(curSpd);

        // Convert absolute step values into correct deg/distance as required

        // create useful time string, with a minimum of two-digit numbers for hours, minutes, and seconds

    QString timeText = QString("%1").arg((unsigned int)TimeConverter::hours(curMs), 2, 10, QChar('0')) + "'"
            + QString("%1").arg((unsigned int)TimeConverter::freeMinutes(curMs), 2, 10, QChar('0')) + "\""
            + QString("%1").arg((unsigned int)TimeConverter::freeSeconds(curMs), 2, 10, QChar('0'));

        // tooltip text
/*    QString descText = MA_STR_POS + QString::number(curPos, 'f', 2) + dLabel + "\n"
            + MA_STR_SPD + QString::number(curSpd, 'f', 2) + dLabel + MA_STR_MOD + "\n"
            + MA_STR_TIM + timeText; */

    QString descText = MA_STR_POS + posDisp[0] + posDisp[1] + "\n"
            + MA_STR_SPD + spdDisp[0] + spdDisp[1] + MA_STR_MOD + "\n"
            + MA_STR_TIM + timeText;

    QToolTip::showText(point, descText);
}

/** Slot for when the track is to be muted. Does not change mute setting if track
    is already muted from an error */

void MotionArea::mute() {
    if( m_sane ) {
        if( m_mute == MA_MUTE_NA ) {
            m_mute = MA_MUTE_MT;
            m_bgCol = MA_MT_COLOR;
        }
        else if( m_mute != MA_MUTE_ER ) {
            m_mute = MA_MUTE_NA;
            m_bgCol = MA_BG_COLOR;
        }
    }

    emit muted(m_mute);
}

/** Slot for Setting Current Play Status

  Used to disable certain inputs when the film is playing */

void MotionArea::playStatus(bool p_stat) {
    m_pstat = p_stat;
}
