#include "motionbase.h"
#include "ui_motionbase.h"

#include <QDebug>

MotionBase::MotionBase(FilmParameters* c_film, OMdeviceInfo* c_dev, AxisOptions *c_aopts, GlobalOptions *c_gopts, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MotionBase)
{
    ui->setupUi(this);

    m_area = new MotionArea(c_film, c_dev, c_aopts, c_gopts, this);
    ui->horizontalLayout->addWidget(m_area);
    ui->horizontalLayout->setContentsMargins(0,0,0,0);
    ui->horizontalLayout->setMargin(0);
    ui->horizontalLayout->setSpacing(0);

    m_film = c_film;
    m_dev = c_dev;

    ui->nameLabel->setText(c_dev->name);
    ui->resButton->setText(" R "); // default is rapid mode

        // pass through click signal
    connect(ui->scaleButton, SIGNAL(clicked()), m_area, SLOT(scaleChange()));
    connect(ui->muteButton, SIGNAL(clicked()), m_area, SLOT(mute()));

    connect(m_area, SIGNAL(scaleChanged(bool)), this, SLOT(curScale(bool)));
    connect(m_area, SIGNAL(muted(int)), this, SLOT(muted(int)));

    connect(m_area, SIGNAL(globalPosition(int,int)), this, SIGNAL(areaBorders(int,int)));
    connect(this, SIGNAL(playStatus(bool)), m_area, SLOT(playStatus(bool)));

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

void MotionBase::on_resButton_clicked() {

    OMfilmParams* parms = m_film->getParams();

    OMfilmAxisParams* axis = parms->axes.value(m_dev->device->address());

    int curRes = axis->ms;

    if( curRes == 1 ) {
        curRes = 2;
        ui->resButton->setText(" C ");
    }
    else if( curRes == 2 ) {
        curRes = 4;
        ui->resButton->setText(" M ");
    }
    else if( curRes == 4 ) {
        curRes = 8;
        ui->resButton->setText(" F ");
    }
    else if( curRes == 8 ){
        curRes = 16;
        ui->resButton->setText("VF ");
    }
    else {
        curRes = 1;
        ui->resButton->setText(" R ");
    }

    axis->ms = curRes;
    m_film->releaseParams();

}

/** This slot is triggered when receiving a muted signal from the MotionArea.

    We use the MotionArea to handle muting, since it controls display of the motion area (colors, etc.)
    and also handles sanity checking on the move its self, which may mute the track.

    We handle updating the button state, and the axis' parameters as needed

    */

void MotionBase::muted(int p_muted) {

    OMfilmParams* parms = m_film->getParams();

    OMfilmAxisParams* axis = parms->axes.value(m_dev->device->address());

    if( p_muted != MA_MUTE_NA ) {
        p_muted = 1;
        ui->muteButton->setDown(true);
    }
    else {
        p_muted = 0;
        ui->muteButton->setDown(false);
    }

    axis->mute = p_muted;
        // we can enter a feedback loop - so don't broadcast.  No one cares about muting
        // except MotionArea, us, and FilmExec.
    m_film->releaseParams(false);

}

/** Receive Playing Status

  This slot receives the current playing status.  We only do something when
  this status actually changes.

  The purpose of monitoring status here is to disable certain inputs when
  the film is playing, as the user should not be able to modify them.
  */

void MotionBase::statusChange(bool p_stat) {

    qDebug() << "MB: Got statChange:" << p_stat;

    ui->muteButton->setEnabled( ! p_stat);
    ui->easeButton->setEnabled( ! p_stat);
    ui->resButton->setEnabled( ! p_stat);

    emit playStatus(p_stat);

}
