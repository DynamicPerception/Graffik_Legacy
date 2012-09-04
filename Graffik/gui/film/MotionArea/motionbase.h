#ifndef MOTIONBASE_H
#define MOTIONBASE_H

#include <QWidget>

#include "motionarea.h"
#include "MoCoBus/omnetwork.h"
#include "core/Options/axisoptions.h"
#include "core/Options/globaloptions.h"
#include "film/FilmParameters/filmparameters.h"

namespace Ui {
class MotionBase;
}

class MotionBase : public QWidget
{
    Q_OBJECT
    
public:
    MotionBase(FilmParameters* c_film, OMdeviceInfo* c_dev, AxisOptions* c_aopts, GlobalOptions* c_gopts, QWidget *parent);
    ~MotionBase();
    
public slots:
    void curScale(bool p_scale);
    void muted(int p_muted);
    void on_easeButton_clicked();
    void on_resButton_clicked();

    void statusChange(bool p_stat);

signals:

        /** Reflected signal from Motion Area, indicates the left X and right X borders
            (in global coordinates) of the area drawn */

    void areaBorders(int p_leftX, int p_rightX);

        /** Reflect Signal */
    void playStatus(bool p_stat);

private:
    Ui::MotionBase *ui;
    MotionArea* m_area;
    FilmParameters* m_film;
    OMdeviceInfo* m_dev;


};

#endif // MOTIONBASE_H
