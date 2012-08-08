#include "motionbase.h"
#include "ui_motionbase.h"


MotionBase::MotionBase(FilmParameters* c_film, OMdeviceInfo* c_dev, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MotionBase)
{
    ui->setupUi(this);

    m_area = new MotionArea(c_film, c_dev, this);
    ui->horizontalLayout->addWidget(m_area);
    ui->horizontalLayout->setContentsMargins(0,0,0,0);
    ui->horizontalLayout->setMargin(0);
    ui->horizontalLayout->setSpacing(0);

    m_film = c_film;

}

MotionBase::~MotionBase()
{
    delete m_area;
    delete ui;
}
