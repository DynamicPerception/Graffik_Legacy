#ifndef MOTIONBASE_H
#define MOTIONBASE_H

#include <QWidget>
#include "motionarea.h"
#include "MoCoBus/omnetwork.h"

#include "film/FilmParameters/filmparameters.h"

namespace Ui {
class MotionBase;
}

class MotionBase : public QWidget
{
    Q_OBJECT
    
public:
    MotionBase(FilmParameters* c_film, OMdeviceInfo *c_dev = 0, QWidget *parent);
    ~MotionBase();
    
private:
    Ui::MotionBase *ui;
    MotionArea* m_area;
    FilmParameters* m_film;
};

#endif // MOTIONBASE_H
