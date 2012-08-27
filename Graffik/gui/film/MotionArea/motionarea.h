#ifndef MOTIONAREA_H
#define MOTIONAREA_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QRect>
#include <QList>
#include <QTimer>
#include <QToolTip>

#include "MoCoBus/omnetwork.h"

#include "film/FilmParameters/filmparameters.h"
#include "core/AxisOptions/axisoptions.h"

#include "motionpathpainter.h"

#define MA_BG_COLOR "#5999BC"
#define MA_CL_COLOR "#FFFFFF"
#define MA_ER_COLOR "#FF3333"

#define MA_PT_NONE  0
#define MA_PT_START 1
#define MA_PT_END   2
#define MA_PT_ACE   3
#define MA_PT_DCS   4

#define MA_TT_TIMER 500

namespace Ui {
class MotionArea;
}

class MotionArea : public QWidget
{
    Q_OBJECT
    
public:
    MotionArea(FilmParameters* c_film, OMdeviceInfo* c_dev, AxisOptions* c_aopt, QWidget *parent);
    ~MotionArea();

    void paintEvent(QPaintEvent* e);

    void mousePressEvent(QMouseEvent* p_event);
    void mouseMoveEvent(QMouseEvent* p_event);
    void mouseReleaseEvent(QMouseEvent* p_event);

    
public slots:
    void filmUpdated();
    void scaleChange();
    void axisOptionsUpdated(OMaxisOptions* p_opts,unsigned short p_addr);
    void moveSane(bool p_sane);

signals:
    void scaleChanged(bool p_scale);

private slots:
    void _tooltipTimer();

private:
    Ui::MotionArea *ui;
    OMdeviceInfo* m_dev;
    FilmParameters* m_film;
    MotionPathPainter* m_path;
    AxisOptions* m_aopt;
    QTimer* m_timer;

    QRect m_mvStart;
    QRect m_mvEnd;
    QRect m_acEnd;
    QRect m_dcStart;

    QString m_bgCol;

    int m_moveItem;
    int m_curPx;
    int m_curPy;

};

#endif // MOTIONAREA_H
