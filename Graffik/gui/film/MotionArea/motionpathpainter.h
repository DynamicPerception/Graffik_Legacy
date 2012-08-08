#ifndef MOTIONPATHPAINTER_H
#define MOTIONPATHPAINTER_H

#include <QPainterPath>
#include "film/FilmParameters/filmparameters.h"

    // rough and dirty travel constants from
    // OMMotor (on AVR)
#define MPP_TRAVEL_LIN      2.0
#define MPP_TRAVEL_QUAD     2.9999985
#define MPP_TRAVEL_QUADINV  1.5000597

class MotionPathPainter
{
    Q_OBJECT
public:
    explicit MotionPathPainter();
    ~MotionPathPainter();
    
    void setMotionPath(unsigned short p_addr, OMfilmParams* p_parms, int p_height, int p_width);
    QPainterPath* getPath();

signals:
    
public slots:
    
private:
    struct s_splineCal {
        float acTm;
        float dcTm;
        float crTm;
        float topSpeed;
        float dcStart;
        float travel;
    };

    s_splineCal m_splinePlanned;

    OMfilmAxisParams* m_parms;
    QPainterPath* m_path;

    void _initSpline(float p_Steps, unsigned long p_Time, unsigned long p_Accel, unsigned long p_Decel, unsigned long p_totalSplines);
    float _qInvCalc(float p_tmPos);
    float _qEaseCalc(float p_tmPos);
    float _linearEasing(float p_tmPos);

};

#endif // MOTIONPATHPAINTER_H
