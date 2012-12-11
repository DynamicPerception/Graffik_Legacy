/*

  Graffik Motion Control Application

  Copyright (c) 2011-2012 Dynamic Perception

 This file is part of Graffik.

    Graffik is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Graffik is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Graffik.  If not, see <http://www.gnu.org/licenses/>.

    */

#include "motionbase.h"
#include "ui_motionbase.h"

#include <QDebug>

MotionBase::MotionBase(FilmParameters* c_film, OMdeviceInfo* c_dev, AxisOptions *c_aopts, GlobalOptions *c_gopts, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MotionBase)
{
    ui->setupUi(this);

    m_area = new MotionArea(c_film, c_dev, c_aopts, c_gopts, this);
    ui->gridLayout->addWidget(m_area, 1, 2, 2, 1);
   // ui->horizontalLayout->setContentsMargins(0,0,0,0);
   // ui->horizontalLayout->setMargin(0);
   // ui->horizontalLayout->setSpacing(0);

    m_film = c_film;
    m_dev = c_dev;

    ui->nameLabel->setText(c_dev->name);
    ui->resButton->setText(" R "); // default is rapid mode
    ui->dispLCD->setToolTip(MB_STR_POS);

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

/** Receive Status On/Off Change

  This slot receives the current playing status when it changes.

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

/** Receive Current Playing Status

  This slot receives the current playing status, and is used to update the
  real-time pos/speed display.  This slot differs from statusChange(), because
  it is not use to set enable/disable, but instead is attached to the PlayMonitor
  by FilmWindow, and is only used to monitor status when playing.
  */

void MotionBase::currentPlayStatus(bool p_stat, unsigned long p_runTime) {
    if( p_stat == true ) {
        int x = m_area->getPathPainter()->getX(p_runTime);
        float spd = m_area->getPathPainter()->getPosition(x);
        qDebug() << "MB: Update Spd Disp: " << x << spd;
        QList<QString> disp = m_area->convertValue(spd);
        ui->dispLCD->display(disp[0]);
        ui->dispLCD->setToolTip(MB_STR_POS + disp[1]);
    }
}
