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
        OMfilmParams* parms = m_film->getParams();
        parms->axes.value(m_dev->device->address())->startTm = startTm;
        m_film->releaseParams();
    }
    else if( m_moveItem == MA_PT_END ) {
        unsigned long endTm = m_path->getFilmTime(newX);
        qDebug() << "MA: Moving End:" << endTm;
        OMfilmParams* parms = m_film->getParams();
        parms->axes.value(m_dev->device->address())->endTm = endTm;
        qDebug() << "MA: Releasing";
        m_film->releaseParams();
        qDebug() << "MA: Released!";
    }
    else if( m_moveItem == MA_PT_ACE ) {
        unsigned long acEnd = m_path->getFilmTime(newX);
        OMfilmParams* parms = m_film->getParams();
        unsigned long newAccel = acEnd - parms->axes.value(m_dev->device->address())->startTm;
        parms->axes.value(m_dev->device->address())->accelTm = newAccel;
        m_film->releaseParams();
    }
    else if( m_moveItem == MA_PT_DCS ) {
        unsigned long dcStart = m_path->getFilmTime(newX);
        OMfilmParams* parms = m_film->getParams();
        unsigned long endTm = parms->axes.value(m_dev->device->address())->endTm;
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

    painter.fillRect(eventRect, QColor(MA_BG_COLOR));
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

 //   _drawCenterLine(&painter, &eventRect);

}

void MotionArea::_drawCenterLine(QPainter *p_paint, QRect *p_rect) {
    QPen midLinePen(QColor(MA_CL_COLOR));
    midLinePen.setWidth(1);

    p_paint->setPen(midLinePen);

    QRect widRect = this->frameGeometry();

    // QList<QPoint> widMidLine = _getVerticalCenterPoints(widRect);
    QList<QPoint> mergePoints = _getVerticalCenterPoints(&widRect);
   // QList<QPoint> eventPoints = _getVerticalCenterPoints(p_rect);

   // QList<QPoint> mergePoints = _restrictPoints(widMidLine, eventPoints);

    if( mergePoints.length() < 2 )
        return;

    p_paint->drawLine(mergePoints[0], mergePoints[1]);

}

QList<QPoint> MotionArea::_getVerticalCenterPoints(QRect *p_rect) {
    QPoint start = p_rect->bottomLeft();
    QPoint end = p_rect->bottomRight();

    int ht = p_rect->height();
    int md = ht / 2;

    start.setY(start.y() - md);
    end.setY(end.y() - md);

    QList<QPoint> ret;

    ret.append(start);
    ret.append(end);

    return ret;
}

/* QList<QPoint> MotionArea::_restrictPoints(QList<QPoint> &p_from, QList<QPoint> &p_into) {



    if( p_from[0].x() < p_into[0].x() )
        p_from[0].x() = p_into[0].x();

    if( p_from[1].x() > p_into[1].x() )
        p_from[1].x() = p_into[1].x();

   // if( p_from[0].y() )
}*/
