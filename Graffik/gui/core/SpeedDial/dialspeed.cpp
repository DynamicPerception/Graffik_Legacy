#include "dialspeed.h"
#include "ui_dialspeed.h"

DialSpeed::DialSpeed(QWidget *parent) :
    QMainWindow(parent, Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint),
    ui(new Ui::DialSpeed)
{
    ui->setupUi(this);
}

DialSpeed::~DialSpeed()
{
    delete ui;
}
