#include "motionarea.h"
#include "ui_motionarea.h"

#include <QPoint>
#include <QPen>

#include <QDebug>


MotionArea::MotionArea(FilmParameters *c_film, OMdeviceInfo *c_dev, AxisOptions* c_aopt, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MotionArea)
{
    m_dev = c_dev;
    m_film = c_film;
    m_aopt = c_aopt;

    m_bgCol = MA_BG_COLOR;
    m_mute  = MA_MUTE_NA;
    m_sane  = true;

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

}

MotionArea::~MotionArea()
{
    delete ui;
    delete m_path;

    m_timer->stop();
    delete m_timer;
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

void MotionArea::axisOptionsUpdated(OMaxisOptions *p_opts, unsigned short p_addr) {
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

    if( m_mvStart.contains(cPos) ) {
        qDebug() << "MA: Clicked MVStart";
        m_moveItem = MA_PT_START;
    }
    else if( m_mvEnd.contains(cPos) ) {
        qDebug() << "MA: Clicked MVEnd";
        m_moveItem = MA_PT_END;
    }
    else if( m_acEnd.contains(cPos) ) {
        qDebug() << "MA: Clicked ACEnd";
        m_moveItem = MA_PT_ACE;
    }
    else if(m_dcStart.contains(cPos) ) {
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


    int newX = p_event->pos().x();

    if( m_moveItem == MA_PT_NONE ) {

        return;
    }
    else {
            // turn off timer,
        m_timer->stop();
    }

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

void MotionArea::mouseReleaseEvent(QMouseEvent *p_event) {
    m_moveItem = MA_PT_NONE;
}

/** Override Paint Event - Draw Motion Path */

void MotionArea::paintEvent(QPaintEvent *e) {
    QPainter painter(this);
    QRect eventRect = e->rect();

    painter.fillRect(eventRect, QColor(m_bgCol));
    painter.drawPath(*m_path->getPath(eventRect));

        // let everyone know where our left and right sides are, so tied elements can line up
    emit globalPosition(mapToGlobal(QPoint(eventRect.topLeft().x(), 0)).x(), mapToGlobal(QPoint(eventRect.topRight().x(), 0)).x());

        // Regenerate grab-and-drag circles if the path has changed

        // TODO: Remove hard-coded sizes.

    if( m_path->hasChanged() ) {
        m_mvStart.setRect(m_path->getStartPx() - 5, eventRect.bottom() - 5, 10, 10);
        m_mvEnd.setRect(m_path->getEndPx() - 5, eventRect.bottom() - 5, 10, 10);
        m_acEnd.setRect(m_path->getAcEndPx() - 5, m_path->getMaxHeight() - 5, 10, 10);
        m_dcStart.setRect(m_path->getDcStartPx() - 5, m_path->getMaxHeight() - 5, 10, 10);
    }

     // draw drag points for start and end time
    if( m_path->isDrawn() ) {
        painter.drawEllipse(m_mvStart);
        painter.drawEllipse(m_mvEnd);
        painter.drawEllipse(m_acEnd);
        painter.drawEllipse(m_dcStart);
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

        // TODO: need to convert absolute step values into correct deg/distance
    // OMaxisOptions* devOpts = m_aopt->getOptions(m_dev->device->address());

    QString descText = "Position: " + QString::number(curPos) + "\n" + "Speed: " + QString::number(curSpd) + "/sec\n" + QString::number(curMs);

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
