#ifndef MOTIONAREA_H
#define MOTIONAREA_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QRect>
#include <QList>

#include "MoCoBus/omnetwork.h"

#define MA_BG_COLOR "#5999BC"
#define MA_CL_COLOR "#FFFFFF"

namespace Ui {
class MotionArea;
}

class MotionArea : public QWidget
{
    Q_OBJECT
    
public:
    MotionArea(OMdeviceInfo* c_dev, QWidget *parent);
    ~MotionArea();

    void paintEvent(QPaintEvent * e);
    
private:
    Ui::MotionArea *ui;
    OMdeviceInfo* m_dev;

    void _drawCenterLine(QPainter* p_paint, QRect* p_rect);
    QList<QPoint> _getVerticalCenterPoints(QRect* p_rect);
  //  QList<QPoint> _restrictPoints(QList<QPoint> &p_from, QList<QPoint> &p_into);

};

#endif // MOTIONAREA_H
