#include "motionarea.h"
#include "ui_motionarea.h"

MotionArea::MotionArea(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MotionArea)
{
    ui->setupUi(this);
}

MotionArea::~MotionArea()
{
    delete ui;
}
