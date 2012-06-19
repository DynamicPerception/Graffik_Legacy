#ifndef MOTIONBASE_H
#define MOTIONBASE_H

#include <QWidget>
#include "motionarea.h"
#include "MoCoBus/omnetwork.h"

namespace Ui {
class MotionBase;
}

class MotionBase : public QWidget
{
    Q_OBJECT
    
public:
    MotionBase(OMdeviceInfo* c_dev, QWidget *parent = 0);
    ~MotionBase();
    
private:
    Ui::MotionBase *ui;
    MotionArea* m_area;
};

#endif // MOTIONBASE_H
