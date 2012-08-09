#ifndef MOTIONBASE_H
#define MOTIONBASE_H

#include <QWidget>

#include "motionarea.h"
#include "MoCoBus/omnetwork.h"
#include "core/AxisOptions/axisoptions.h"
#include "film/FilmParameters/filmparameters.h"

namespace Ui {
class MotionBase;
}

class MotionBase : public QWidget
{
    Q_OBJECT
    
public:
    MotionBase(FilmParameters* c_film, OMdeviceInfo* c_dev, AxisOptions* c_aopts, QWidget *parent);
    ~MotionBase();
    
public slots:
    void curScale(bool p_scale);
    void on_easeButton_clicked();

private:
    Ui::MotionBase *ui;
    MotionArea* m_area;
    FilmParameters* m_film;
    OMdeviceInfo* m_dev;

};

#endif // MOTIONBASE_H
