#include "motionarea.h"
#include "ui_motionarea.h"

#include <QPoint>
#include <QPen>

#include <QDebug>



MotionArea::MotionArea(OMdeviceInfo *c_dev, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MotionArea)
{
    m_dev = c_dev;
    ui->setupUi(this);
}

MotionArea::~MotionArea()
{
    delete ui;
}


void MotionArea::paintEvent(QPaintEvent *e) {
    QPainter painter(this);
    QRect widRec = e->rect();

    painter.fillRect(widRec, QColor(MA_BG_COLOR));

    _drawCenterLine(&painter, &widRec);

}

void MotionArea::_drawCenterLine(QPainter *p_paint, QRect *p_rect) {
    QPen midLinePen(QColor(MA_CL_COLOR));
    midLinePen.setWidth(3);

    p_paint->setPen(midLinePen);


    QList<QPoint> linePoints = _getVerticalCenterPoints(p_rect);

    if( linePoints.length() < 2 )
        return;

    p_paint->drawLine(linePoints[0], linePoints[1]);

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
