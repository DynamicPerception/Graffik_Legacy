#ifndef MOTIONAREA_H
#define MOTIONAREA_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QRect>
#include <QList>

#include "MoCoBus/omnetwork.h"

#include "film/FilmParameters/filmparameters.h"
#include "core/AxisOptions/axisoptions.h"

#include "motionpathpainter.h"

#define MA_BG_COLOR "#5999BC"
#define MA_CL_COLOR "#FFFFFF"

namespace Ui {
class MotionArea;
}

class MotionArea : public QWidget
{
    Q_OBJECT
    
public:
    MotionArea(FilmParameters* c_film, OMdeviceInfo* c_dev, AxisOptions* c_aopt, QWidget *parent);
    ~MotionArea();

    void paintEvent(QPaintEvent * e);
    
public slots:
    void filmUpdated();
    void scaleChange();

signals:
    void scaleChanged(bool p_scale);

private:
    Ui::MotionArea *ui;
    OMdeviceInfo* m_dev;
    FilmParameters* m_film;
    MotionPathPainter* m_path;
    AxisOptions* m_aopt;

    void _drawCenterLine(QPainter* p_paint, QRect* p_rect);
    QList<QPoint> _getVerticalCenterPoints(QRect* p_rect);
  //  QList<QPoint> _restrictPoints(QList<QPoint> &p_from, QList<QPoint> &p_into);

};

#endif // MOTIONAREA_H
