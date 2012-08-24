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

    ui->setupUi(this);

    ui->gridLayout->setContentsMargins(0,0,0,0);
    ui->gridLayout->setMargin(0);
    ui->gridLayout->setSpacing(0);

    m_path = new MotionPathPainter(c_dev->device->address(), m_film, m_aopt, this);
    m_path->setMotionCurve();
    m_path->scaling(false);

    m_moveItem = MA_PT_NONE;

    connect(m_film, SIGNAL(paramsReleased()), this, SLOT(filmUpdated()));
    connect(m_aopt, SIGNAL(deviceOptionsChanged(OMaxisOptions*,unsigned short)), this, SLOT(axisOptionsUpdated(OMaxisOptions*,unsigned short)));
    connect(m_path, SIGNAL(moveSane(bool)), this, SLOT(moveSane(bool)));

}

MotionArea::~MotionArea()
{
    delete ui;
    delete m_path;
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

void MotionArea::axisOptionsUpdated(OMaxisOptions *p_opts, unsigned short p_addr) {
    if( p_addr != m_dev->device->address() )
        return;

    qDebug() << "MA: Got Axis Options Change";
    this->update();
}


void MotionArea::mousePressEvent(QMouseEvent *p_event) {
    QPoint cPos = p_event->pos();

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
    }

}

void MotionArea::mouseMoveEvent(QMouseEvent *p_event) {
    if( m_moveItem == MA_PT_NONE )
        return;

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
        unsigned long dcTm = m_path->getFilmTime(m_path->getDcStartPx());

        OMfilmParams* parms = m_film->getParams();
        OMfilmAxisParams* axis = parms->axes.value(m_dev->device->address());

        qDebug() << "MA: AC: " << acEnd << dcTm;

        if( acEnd >= dcTm )
            acEnd = dcTm - 10;

        unsigned long newAccel = acEnd - axis->startTm;
        axis->accelTm = newAccel;
        m_film->releaseParams();
    }
    else if( m_moveItem == MA_PT_DCS ) {
        unsigned long dcStart = m_path->getFilmTime(newX);
        unsigned long acTm = m_path->getFilmTime(m_path->getAcEndPx());

        OMfilmParams* parms = m_film->getParams();
        OMfilmAxisParams* axis = parms->axes.value(m_dev->device->address());

        qDebug() << "MA: DC: " << dcStart << acTm;

        if( dcStart <= acTm )
            dcStart = acTm + 10;

        unsigned long endTm = axis->endTm;
        endTm = endTm > 0 ? endTm : parms->realLength;
        unsigned long newDecel = endTm - dcStart;
        parms->axes.value(m_dev->device->address())->decelTm = newDecel;
        m_film->releaseParams();
    }

  //  this->update();
}

void MotionArea::mouseReleaseEvent(QMouseEvent *p_event) {
    m_moveItem = MA_PT_NONE;
}

void MotionArea::paintEvent(QPaintEvent *e) {
    QPainter painter(this);
    QRect eventRect = e->rect();

    painter.fillRect(eventRect, QColor(m_bgCol));
    painter.drawPath(*m_path->getPath(eventRect));


        // only update drag point position if the pather path has
        // changed

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

void MotionArea::moveSane(bool p_sane) {

    qDebug() << "MA: Got Sane: " << p_sane;
    if( p_sane )
        m_bgCol = MA_BG_COLOR;
    else
        m_bgCol = MA_ER_COLOR;
}
