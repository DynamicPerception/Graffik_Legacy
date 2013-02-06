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


MotionArea::MotionArea(FilmParameters *c_film, OMdeviceInfo *c_dev, AxisOptions* c_aopt, GlobalOptions *c_gopt, QWidget *parent) : QFrame(parent), ui(new Ui::MotionArea) {

        // handle QGradient metatype registration for our property
    qRegisterMetaType<QLinearGradient>("QLinearGradient");

    m_dev = c_dev;
    m_film = c_film;
    m_aopt = c_aopt;
    m_gopt = c_gopt;

    m_mute  = MA_MUTE_NA;
    m_sane  = true;

    m_lineWidth = 1;
    m_lineColor = Qt::black;
    m_fillStartColor = Qt::black;
    m_fillEndColor = Qt::white;
    m_grabLineColor = Qt::black;
    m_grabFillColor = Qt::white;

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
    setMouseTracking(true);

    setStyleSheet(SingleThemer::getStyleSheet("motionarea"));

}

MotionArea::~MotionArea() {
    delete ui;
    delete m_path;

    m_timer->stop();
    delete m_timer;
}


QColor MotionArea::lineColor() {
    return m_lineColor;
}

void MotionArea::setLineColor(QColor p_color) {
    m_lineColor = p_color;
}

QColor MotionArea::fillColorStart() {
    return m_fillStartColor;
}

void MotionArea::setFillColorStart(QColor p_color) {
    m_fillStartColor = p_color;
}

QColor MotionArea::fillColorEnd() {
    return m_fillEndColor;
}

void MotionArea::setFillColorEnd(QColor p_color) {
    m_fillEndColor = p_color;
}

QColor MotionArea::grabLineColor() {
    return m_grabLineColor;
}

void MotionArea::setGrabLineColor(QColor p_color) {
    m_grabLineColor = p_color;
}

QColor MotionArea::grabFillColor() {
    return m_grabFillColor;
}

void MotionArea::setGrabFillColor(QColor p_color) {
    m_grabFillColor = p_color;
}

int MotionArea::lineWidth() {
    return m_lineWidth;
}

void MotionArea::setLineWidth(int p_width) {
    m_lineWidth = p_width;
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

        // sanity check
    if( newX < 0 || newX > rect().width() )
        return;

    if( m_moveItem == MA_PT_START ) {


            // user is moving start time
        unsigned long startTm = m_path->getFilmTime(newX);
        unsigned long    acTm = m_path->getFilmTime(m_path->getAcEndPx());
        unsigned long    dcTm = m_path->getFilmTime(m_path->getDcStartPx());

        OMfilmParams*    parms = m_film->getParams();
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
            // user is moving end time

        unsigned long endTm = m_path->getFilmTime(newX);
        unsigned long  acTm = m_path->getFilmTime(m_path->getAcEndPx());
        unsigned long  dcTm = m_path->getFilmTime(m_path->getDcStartPx());

        OMfilmParams* parms = m_film->getParams();
        OMfilmAxisParams* axis = parms->axes.value(m_dev->device->address());


        if( endTm < axis->endTm ) {
            if( dcTm - (axis->endTm - endTm) < acTm )
                endTm = axis->endTm;
        }

        axis->endTm = endTm;
        m_film->releaseParams();
    }
    else if( m_moveItem == MA_PT_ACE ) {
            // user is moving acceleration end time
        float acEnd = m_path->getFilmTime(newX);
        float  dcTm = m_path->getFilmTime(m_path->getDcStartPx());


        OMfilmParams*    parms = m_film->getParams();
        OMfilmAxisParams* axis = parms->axes.value(m_dev->device->address());

            // limit move to be rational to other points
        if( acEnd >= dcTm ) {
            acEnd = dcTm - 1;
        }

        if( acEnd <= axis->startTm ) {
            acEnd = axis->startTm + 100;
        }

        axis->accelTm = acEnd - axis->startTm;
        m_film->releaseParams();

    }
    else if( m_moveItem == MA_PT_DCS ) {
            // user is moving deceleration start time
        unsigned long dcStart = m_path->getFilmTime(newX);
        unsigned long    acTm = m_path->getFilmTime(m_path->getAcEndPx());
        unsigned long   endTm = m_path->getFilmTime(m_path->getEndPx());

        OMfilmParams*    parms = m_film->getParams();
        OMfilmAxisParams* axis = parms->axes.value(m_dev->device->address());

            // limit move the ac end point
        if( dcStart <= acTm ) {
            dcStart = acTm + 1;
        }

        if( dcStart >= endTm ) {
            dcStart = endTm - 100;
        }

        axis->decelTm = endTm - dcStart;
        m_film->releaseParams();
    }

    // this->update();
}

/** Handle mouse Release Events inside of widget */

void MotionArea::mouseReleaseEvent(QMouseEvent *) {
    m_moveItem = MA_PT_NONE;
}

/** Override Paint Event - Draw Motion Path */

void MotionArea::paintEvent(QPaintEvent *e) {

    Q_UNUSED(e);

    QPainter painter(this);

        // we need the rectangle of the widget, not the
        // event, so that our line drawing doesn't skew to
        // a sub-section of the display area when it is
        // partially hidden, such as when scrolling

    QRect eventRect = rect();

    int baseline = eventRect.height() * MPP_HEIGHT_BUF;

        // fill gradient

    QLinearGradient grad(QPointF(0, baseline / 2), QPointF(0, eventRect.height()));

    grad.setColorAt(0, m_fillStartColor);
    grad.setColorAt(1, m_fillEndColor);

    QBrush brush(grad);
    QPen   pen(m_lineColor);

    pen.setWidth(m_lineWidth);

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(pen);
    painter.setBrush(brush);

    painter.drawPath(*m_path->getPath(eventRect));

        // Filling the path above leaves a blank space in the buffer area, so
        // fill area below line with gradient, for a better look

    if( m_path->getMaxSpeed() > 0.0 )
        painter.fillRect(0, baseline, eventRect.width(), eventRect.height() - baseline, brush);

        // let everyone know where our left and right sides are, so tied elements can line up
    emit globalPosition(mapToGlobal(QPoint(eventRect.topLeft().x(), 0)).x(), mapToGlobal(QPoint(eventRect.topRight().x(), 0)).x());

        // Regenerate grab-and-drag circles if the path has changed, but only
        // do so if the film is not playing

        // TODO: Remove hard-coded sizes.

    if( m_pstat == false ) {
        if( m_path->hasChanged() ) {

            m_mvStart.setRect(m_path->getStartPx() - 5, baseline  - 5, 10, 10);
            m_mvEnd.setRect(m_path->getEndPx() - 5, baseline - 5, 10, 10);
            m_acEnd.setRect(m_path->getAcEndPx() - 5, m_path->getMaxHeight() - 5, 10, 10);
            m_dcStart.setRect(m_path->getDcStartPx() - 5, m_path->getMaxHeight() - 5, 10, 10);
        }

         // draw drag points for start and end time
        if( m_path->isDrawn() && QWidget::underMouse() ) {
            QPen gPen(m_grabLineColor);
            QBrush gBrush(m_grabFillColor);

            painter.setPen(gPen);
            painter.setBrush(gBrush);
            painter.drawEllipse(m_mvStart);
            painter.drawEllipse(m_mvEnd);
            painter.drawEllipse(m_acEnd);
            painter.drawEllipse(m_dcStart);
        }
    }

}

/** Slot For Sane Movement Signal from MotionPathPainter */

void MotionArea::moveSane(bool p_sane) {

    qDebug() << "MA: Got moveSane" << p_sane;

        // handle mute check for bg color
    if( p_sane ) {
            // do not change mute setting on sane check, unless it was
            // set to error by this function
        if( m_mute == MA_MUTE_ER  || m_mute == MA_MUTE_NA ) {
            m_mute = MA_MUTE_NA;
            _refreshStylesheet();
        }

    }
    else if( m_mute != MA_MUTE_MT ) {
            // do not mute as error if track already muted
       m_mute = MA_MUTE_ER;
       _refreshStylesheet();
    }

        // inform other components that this track is either
        // muted, or un-muted.

    m_sane = p_sane;
        // careful of feedback loop - do not let any slot
        // attached to this attempt to broadcast a change to
        // film parameters

    qDebug() << "MA: Emitting muted()";

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

        // Convert absolute step values into correct deg/distance as required

    QList<QString> posDisp = convertValue(curPos);
    QList<QString> spdDisp = convertValue(curSpd);

        // create useful time string, with a minimum of two-digit numbers for hours, minutes, and seconds

    QString timeText = TimeConverter::stringify(curMs);

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
            _refreshStylesheet();
        }
        else if( m_mute != MA_MUTE_ER ) {
            m_mute = MA_MUTE_NA;
            _refreshStylesheet();
        }
    }

    emit muted(m_mute);
}

int MotionArea::muted() {
    if( m_mute == MA_MUTE_MT )
        return 1;
    else if( m_mute == MA_MUTE_ER )
        return 2;
    else
        return 0;
}

void MotionArea::_refreshStylesheet() {
    style()->unpolish(this);
    style()->polish(this);
    update();
}

/** Slot for Setting Current Play Status

  Used to disable certain inputs when the film is playing */

void MotionArea::playStatus(bool p_stat) {
    m_pstat = p_stat;
}
