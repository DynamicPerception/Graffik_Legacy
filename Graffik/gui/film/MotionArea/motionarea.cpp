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

    connect(m_film, SIGNAL(paramsReleased()), this, SLOT(filmUpdated()));

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

void MotionArea::paintEvent(QPaintEvent *e) {
    QPainter painter(this);
    QRect eventRect = e->rect();

    painter.fillRect(eventRect, QColor(MA_BG_COLOR));
    painter.drawPath(*m_path->getPath(eventRect));

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
