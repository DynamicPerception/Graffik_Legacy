#include "motionbase.h"
#include "ui_motionbase.h"


MotionBase::MotionBase(FilmParameters* c_film, OMdeviceInfo* c_dev, AxisOptions *c_aopts, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MotionBase)
{
    ui->setupUi(this);

    m_area = new MotionArea(c_film, c_dev, c_aopts, this);
    ui->horizontalLayout->addWidget(m_area);
    ui->horizontalLayout->setContentsMargins(0,0,0,0);
    ui->horizontalLayout->setMargin(0);
    ui->horizontalLayout->setSpacing(0);

    m_film = c_film;
    m_dev = c_dev;

    ui->nameLabel->setText(c_dev->name);

        // pass through click signal
    connect(ui->scaleButton, SIGNAL(clicked()), m_area, SLOT(scaleChange()));
    connect(m_area, SIGNAL(scaleChanged(bool)), this, SLOT(curScale(bool)));

}

MotionBase::~MotionBase()
{
    delete m_area;
    delete ui;
}

void MotionBase::curScale(bool p_scale) {
    if( p_scale == true )
        ui->scaleButton->setText("REL");
    else
        ui->scaleButton->setText("PRO");
}

void MotionBase::on_easeButton_clicked() {
    OMfilmParams* parms = m_film->getParams();

    OMfilmAxisParams* axis = parms->axes.value(m_dev->device->address());

    int curEase = axis->easing;

    if( curEase == OM_MOT_QUAD ) {
        curEase = OM_MOT_QUADINV;
        ui->easeButton->setText(" I ");
    }
    else if( curEase == OM_MOT_QUADINV ) {
        curEase = OM_MOT_LINEAR;
        ui->easeButton->setText(" L ");
    }
    else {
        curEase = OM_MOT_QUAD;
        ui->easeButton->setText(" Q ");
    }

    axis->easing = curEase;
    m_film->releaseParams();

}
