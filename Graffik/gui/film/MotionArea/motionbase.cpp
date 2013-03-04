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

MotionBase::MotionBase(FilmParameters* c_film, OMdeviceInfo* c_dev, AxisOptions *c_aopts, GlobalOptions *c_gopts, QWidget *parent) : QWidget(parent), ui(new Ui::MotionBase) {
    ui->setupUi(this);

    m_area = new MotionArea(c_film, c_dev, c_aopts, c_gopts, this);
    ui->gridLayout->addWidget(m_area, 0, 2, 1, 1);

    m_opts = c_aopts;
    m_film = c_film;
     m_dev = c_dev;
    m_addr = m_dev->device->address();


    // OSX has issues laying out these buttons w/o overlap,
    // this is a work-around
#ifdef Q_WS_MAC
    ui->scaleButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);
    ui->resButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);
    ui->easeButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);
    ui->muteButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);
    ui->delButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);
    ui->errorButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);
    ui->masterButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);
#endif

    ui->nameButton->setText(c_dev->name);
    ui->resButton->setText(" R "); // default is rapid mode
    ui->scaleButton->setText(" P ");

    ui->dispLCD->setToolTip(MB_STR_POS);

        // by default this is hidden
    ui->errorButton->hide();

        // set correct state for master button
    OMaxisOptions* opts = m_opts->getOptions(m_addr);

    if( opts->master == true ) {
        ui->masterButton->setState(1);
        ui->masterButton->setToolTip(MB_STR_ISM);
    }
    else {
        ui->masterButton->setState(0);
        ui->masterButton->setToolTip(MB_STR_NTM);
    }

        // pass through click signal
    connect(ui->scaleButton, SIGNAL(clicked()), m_area, SLOT(scaleChange()));
    connect(ui->muteButton, SIGNAL(clicked()), m_area, SLOT(mute()));

    connect(m_area, SIGNAL(scaleChanged(bool)), this, SLOT(curScale(bool)));
    connect(m_area, SIGNAL(muted(int)), this, SLOT(muted(int)));

    connect(m_area, SIGNAL(globalPosition(int,int)), this, SIGNAL(areaBorders(int,int)));
    connect(this, SIGNAL(playStatus(bool)), m_area, SLOT(playStatus(bool)));

    connect(m_opts, SIGNAL(deviceOptionsChanged(OMaxisOptions*,unsigned short)), this, SLOT(deviceOptionsChanged(OMaxisOptions*,unsigned short)));

    // theming

    Themer* theme = &Singleton<Themer>::Instance();
    connect(theme, SIGNAL(themeChanged()), this, SLOT(_themeChanged()));

    setStyleSheet(theme->getThemeCSS("motionbase"));

}

MotionBase::~MotionBase() {
    delete m_area;
    delete ui;
}

void MotionBase::_themeChanged() {
    setStyleSheet(SingleThemer::getStyleSheet("motionbase"));
    style()->unpolish(this);
    style()->polish(this);
    update();
}

/** Get Pointer to Contained MotionArea */

MotionArea* MotionBase::area() {
    return m_area;
}

/** Set Scale Ratio Slot */

void MotionBase::curScale(bool p_scale) {
    if( p_scale == true )
        ui->scaleButton->setText(" R ");
    else
        ui->scaleButton->setText(" P ");
}

/** Delete Motion Button Clicked Slot */

void MotionBase::on_delButton_clicked() {
    ConfirmDialog dia(MB_STR_DEL, this);
    int res = dia.exec();

    if( res == QDialog::Accepted ) {
            // if accepted, purge move information for axis
        OMfilmParams* parms = m_film->getParams();
        OMfilmAxisParams* axis = parms->axes.value(m_dev->device->address());

        axis->endDist = 0;
        axis->decelTm = 0;
        axis->endTm   = 0;
        axis->startTm = 0;
        axis->accelTm = 0;

        m_film->releaseParams();
    }
}

/** Easing Button Clicked Slot */

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

/** Resolution Button Clicked Slot */

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
        ui->resButton->setText(" X ");
    }
    else {
        curRes = 1;
        ui->resButton->setText(" R ");
    }

    axis->ms = curRes;
    m_film->releaseParams();

}

 /** Error Button Clicked Slot */
void MotionBase::on_errorButton_clicked() {
    qDebug() << "MB: Show Error Console";
    MotionErrorDialog err(m_opts->errors(m_dev->device->address()));
    err.exec();
}

/** This slot is triggered when receiving a muted signal from the MotionArea.

    We use the MotionArea to handle muting, since it controls display of the motion area (colors, etc.)
    and also handles sanity checking on the move its self, which may mute the track.

    We handle updating the button state, and the axis' parameters as needed

    */

void MotionBase::muted(int p_muted) {

    OMfilmParams* parms = m_film->getParams();

    OMfilmAxisParams* axis = parms->axes.value(m_dev->device->address());

    qDebug() << "MB: Got mute:" << p_muted;

    if( p_muted != MA_MUTE_NA ) {
        ui->muteButton->setDown(true);

        if( p_muted == MA_MUTE_ER ) {
            ui->errorButton->show();
            ui->errorButton->setEnabled(true);
        }

        p_muted = 1;

    }
    else {
        p_muted = 0;
        ui->muteButton->setDown(false);
        ui->errorButton->hide();
        ui->errorButton->setEnabled(false);
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

            // re-cast as positive value
        spd = spd < 0.0 ? spd * -1.0 : spd;

        QList<QString> disp = m_area->convertValue(spd);
        ui->dispLCD->display(disp[0]);
        ui->dispLCD->setToolTip(MB_STR_POS + disp[1]);
    }
}

void MotionBase::on_nameButton_clicked() {
    OMfilmParams parms = m_film->getParamsCopy();

    OMfilmAxisParams* axis = parms.axes.value(m_dev->device->address());

    TrackInfoDialog dia(axis, this);
    dia.exec();
}

/** Receive Changes to Axis Options

    This slot is triggered when axis options are changed, will correctly update
    the state of the master status button as needed, and refresh the CSS
*/

void MotionBase::deviceOptionsChanged(OMaxisOptions *p_opts, unsigned short p_addr) {

    if( p_addr != m_addr )
        return;

    qDebug() << "MB: Got Options Change for" << p_addr;

        // set master button state correctly on options change
    if( p_opts->master == true ) {
        ui->masterButton->setState(1);
        ui->masterButton->setToolTip(MB_STR_ISM);
    }
    else {
        ui->masterButton->setState(0);
        ui->masterButton->setToolTip(MB_STR_NTM);
    }

        // refresh css
    ui->masterButton->style()->unpolish(ui->masterButton);
    ui->masterButton->style()->polish(ui->masterButton);
    update();
}


void MotionBase::on_masterButton_clicked() {

    OMaxisOptions* opts = m_opts->getOptions(m_addr);

    qDebug() << "MB: Got Click on MasterButton";

        // we don't unset master this way
    if( opts->master == true )
        return;
    else
        m_opts->setMaster(m_addr);

}
