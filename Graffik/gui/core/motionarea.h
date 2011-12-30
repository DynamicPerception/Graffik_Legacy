#ifndef MOTIONAREA_H
#define MOTIONAREA_H

#include <QWidget>

namespace Ui {
class MotionArea;
}

class MotionArea : public QWidget
{
    Q_OBJECT
    
public:
    explicit MotionArea(QWidget *parent = 0);
    ~MotionArea();
    
private:
    Ui::MotionArea *ui;
};

#endif // MOTIONAREA_H
