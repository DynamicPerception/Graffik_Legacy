#ifndef DIALSPEED_H
#define DIALSPEED_H

#include <QMainWindow>

namespace Ui {
    class DialSpeed;
}

class DialSpeed : public QMainWindow
{
    Q_OBJECT

public:
    explicit DialSpeed(QWidget *parent = 0);
    ~DialSpeed();

private:
    Ui::DialSpeed *ui;
};

#endif // DIALSPEED_H
