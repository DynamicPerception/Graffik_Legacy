#ifndef MOTIONERRORELEMENT_H
#define MOTIONERRORELEMENT_H

#include <QWidget>

#include "core/Themer/singlethemer.h"

namespace Ui {
class MotionErrorElement;
}

class MotionErrorElement : public QWidget
{
    Q_OBJECT
    
public:
    MotionErrorElement(QString c_err, QWidget *parent);
    ~MotionErrorElement();
    
private:
    Ui::MotionErrorElement *ui;
};

#endif // MOTIONERRORELEMENT_H
