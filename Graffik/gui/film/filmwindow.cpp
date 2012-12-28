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

#include "filmwindow.h"
#include "ui_filmwindow.h"

#include <QDebug>


FilmWindow::FilmWindow(OMNetwork* c_net, AxisOptions *c_opts, GlobalOptions *c_gopts, QWidget *parent) : QWidget(parent), ui(new Ui::FilmWindow) {

    ui->setupUi(this);

    m_net = c_net;
    m_opts = c_opts;
    m_gopts = c_gopts;

    m_error = false;
    m_spinsPrepped = false;
    m_ignoreUpdate = false;

    m_params = new FilmParameters(m_net, this);
    m_exec = new FilmExec(m_net, m_params, m_opts, m_gopts, this);
    m_busy = new QProgressDialog(this);
    m_jcp = new JogControlPanel(m_net, m_opts, m_params, this);
    m_fcp = new FlickCharm;

    m_areaLayout = new QVBoxLayout;
    m_areaLayout->setContentsMargins(0, 0, 0, 0);
    m_areaLayout->setSpacing(0);

    m_areaViewPort = new QWidget;
    m_areaViewPort->setLayout(m_areaLayout);

    ui->visualSA->setWidget(m_areaViewPort);

        // create our transparent overlay for drawing position line
        // this must be done after the layout is added to visualSAContents,
        // as this class attempts to access that layout

    m_motion = new MotionSection(m_exec, m_params, m_areaViewPort);
    m_filter = new SectionResizeFilter(m_motion, this);

    m_motion->show();

        // need to capture resize events to resize our transparent overlay
    m_areaViewPort->installEventFilter(m_filter);

    m_tape = new MotionTape(m_params, m_areaViewPort, this);
    ui->tapeVLayout->addWidget(m_tape);

    ui->pluginStackedWidget->addWidget(m_jcp);
        // simulate jog plugin button clicked
    on_plugJogButton_clicked();

        // we need to populate motion area displays
    connect(m_net, SIGNAL(deviceAdded(OMdeviceInfo*)), this, SLOT(_drawNewAxis(OMdeviceInfo*)));
    connect(m_net, SIGNAL(deviceDeleted(QString,unsigned short)), this, SLOT(_eraseAxis(QString,unsigned short)));


    connect(m_exec, SIGNAL(filmPlayStatus(bool,ulong)), this, SLOT(_playStatus(bool,ulong)));
    connect(m_exec, SIGNAL(filmStarted()), this, SLOT(_filmStarted()));
    connect(m_exec, SIGNAL(error(QString)), this, SLOT(error(QString)));
    connect(m_exec, SIGNAL(shuttleComplete()), this, SLOT(_shuttleComplete()));

    connect(m_busy, SIGNAL(canceled()), this, SLOT(_busyCanceled()));

    connect(this, SIGNAL(motionAreaBorders(int,int)), m_tape, SLOT(setBorders(int,int)));
    connect(this, SIGNAL(motionAreaBorders(int,int)), m_motion, SLOT(setBorders(int,int)));

    connect(m_params, SIGNAL(paramsReleased()), this, SLOT(filmParamsChanged()));

    _prepInputs();

}

FilmWindow::~FilmWindow() {

    foreach(unsigned short addr, m_areaBlocks.keys()) {
        delete m_areaBlocks.value(addr);
        m_areaBlocks.remove(addr);
    }

    delete m_tape;
    delete m_areaLayout;
    delete m_motion;
    delete m_filter;
   // delete m_areaViewPort;

    delete m_exec;
    delete m_jcp;
    delete m_fcp;
    delete m_params;
    delete m_areaViewPort;
    delete ui;
}


void FilmWindow::_drawNewAxis(OMdeviceInfo *p_dev) {

        // don't show a motion area for device types that
        // do not support it

    if( p_dev->type != "OpenMoCo Axis")
        return;

    qDebug() << "FW: Got new axis, spawning motion area" << p_dev->type;

    MotionBase* area = new MotionBase(m_params, p_dev, m_opts, m_gopts, this);
    m_areaBlocks.insert(p_dev->device->address(), area);
    m_areaLayout->addWidget(area);

        // reflect signal from new motion area
    connect(area, SIGNAL(areaBorders(int,int)), this, SIGNAL(motionAreaBorders(int,int)));
    connect(this, SIGNAL(playStatusChange(bool)), area, SLOT(statusChange(bool)));
    connect(m_exec, SIGNAL(filmPlayStatus(bool,ulong)), area, SLOT(currentPlayStatus(bool,ulong)));

    _redrawMotionOverlay();

}

void FilmWindow::_eraseAxis(QString p_bus, unsigned short p_addr) {
    qDebug() << "FW: Got erase axis" << p_bus << p_addr;

    if( m_areaBlocks.contains(p_addr) ) {
        m_areaLayout->removeWidget(m_areaBlocks.value(p_addr));

        delete m_areaBlocks.value(p_addr);
        m_areaBlocks.remove(p_addr);

        _redrawMotionOverlay();
    }


}

void FilmWindow::on_camControlSlider_selected(int p_value) {

    bool camControl = false;

    if( p_value == 1)
        camControl = true;

    qDebug() << "FW: Cam Control Set to: " << camControl;

    _enableCamControl(camControl);

}

void FilmWindow::_enableCamControl(bool p_en) {

    OMfilmParams* params = m_params->getParams();
    params->camParams->camControl = p_en;
    m_params->releaseParams();

    _displayCamControl();


}

void FilmWindow::_displayCamControl() {

    OMfilmParams parms = m_params->getParamsCopy();

    bool en = parms.camParams->camControl;
    bool autoFPS = parms.camParams->autoFPS;

    if( en )
        ui->camControlSlider->setValue(1);
    else
        ui->camControlSlider->setValue(0);

    ui->camSetBut->setEnabled(en);

        // disable film time spinners unless auto fps is enabled
    if( autoFPS && en ) {
        en = true;
        _checkFilmTimeConstraint();
    }
    else
        en = false;

    ui->filmHHSpin->setEnabled(en);
    ui->filmMMSpin->setEnabled(en);
    ui->filmSSSpin->setEnabled(en);

    _inputEnable(true);

}

// handle clicking the camera settings button

void FilmWindow::on_camSetBut_clicked() {
    CameraControlDialog* control = new CameraControlDialog(m_params);
    control->exec();

    OMfilmParams* params = m_params->getParams();
    bool autoFPS = params->camParams->autoFPS;
    m_params->releaseParams(false);


        // do we need to enable film control spinners?
    bool en = false;

        // if not using auto fps, show what film time will be
        // based on FPS or manual interval
    if( ! autoFPS )
        _calcAutoFilmTime();
    else {
        _checkFilmTimeConstraint();
        en = true;
    }

    qDebug() << "FW: Auto Time: " << autoFPS << en;

    ui->filmHHSpin->setEnabled(en);
    ui->filmMMSpin->setEnabled(en);
    ui->filmSSSpin->setEnabled(en);



    delete control;
}

void FilmWindow::_showFilmTime() {

    qDebug() << "FW: Show Film Time";

    OMfilmParams* params = m_params->getParams();
    unsigned long wallTm = params->realLength;
    unsigned long filmTm = params->length;
    m_params->releaseParams(false);

        // convert from mS to hours, minutes, and seconds

    unsigned long whh = TimeConverter::hours(wallTm);
    unsigned long wmm = TimeConverter::freeMinutes(wallTm);
    unsigned long wss = TimeConverter::freeSeconds(wallTm);

    unsigned long fhh = TimeConverter::hours(filmTm);
    unsigned long fmm = TimeConverter::freeMinutes(filmTm);
    unsigned long fss = TimeConverter::freeSeconds(filmTm);


    m_ignoreUpdate = true;

    ui->filmHHSpin->setValue(fhh);
    ui->filmMMSpin->setValue(fmm);
    ui->filmSSSpin->setValue(fss);

    ui->realHHSpin->setValue(whh);
    ui->realMMSpin->setValue(wmm);
    ui->realSSSpin->setValue(wss);

    m_ignoreUpdate = false;

}

void FilmWindow::_prepInputs() {

    if( ! m_spinsPrepped ) {
        ui->filmHHSpin->setCount(999);
        ui->filmMMSpin->setCount(59);
        ui->filmSSSpin->setCount(59);

        ui->realHHSpin->setCount(999);
        ui->realMMSpin->setCount(59);
        ui->realSSSpin->setCount(59);

        m_spinsPrepped = true;
    }

    _displayCamControl();
    _inputEnable(true);
    _showFilmTime();


}

 // convert inputs to mS time and write back to film parameters
void FilmWindow::_changeTime(int p_which, int p_pos, int p_val) {

    int hh; int mm; int ss;


    if( p_which == 1 ) {
        hh = ui->filmHHSpin->value();
        mm = ui->filmMMSpin->value();
        ss = ui->filmSSSpin->value();
    }
    else {
        hh = ui->realHHSpin->value();
        mm = ui->realMMSpin->value();
        ss = ui->realSSSpin->value();


        qDebug() << "FW: _cT:" << hh << mm << ss;
    }

    if( p_pos == 1 )
        hh = p_val;
    else if( p_pos == 2 )
        mm = p_val;
    else
        ss = p_val;

        // can't have less than one second of real time, we just wouldn't know
        // what to do with ourselves!

    if( hh == 0 && mm == 0 && ss == 0 ) {
            ss = 1;
            if( p_which == 2 )
                ui->realSSSpin->setValue(1);
            else
                ui->filmSSSpin->setValue(1);
     }

    unsigned long mS = TimeConverter::msFromHours(hh) + TimeConverter::msFromMinutes(mm) + TimeConverter::msFromSeconds(ss);
    OMfilmParams* params = m_params->getParams();

    bool autoFPS = params->camParams->autoFPS;
    bool camEn   = params->camParams->camControl;
    bool showUpdate = false;

    if( p_which == 1 )
        params->length = mS;
    else {
        unsigned long oldTm = params->realLength;

        params->realLength = mS;

        if( ! camEn ) {
            qDebug() << "FW: Cam not enabled, set film time to same as real time";
            params->length = mS;
            showUpdate = true;
        }

        float timeDiff = (float) mS / (float) oldTm;


        // can't have axis moves end beyond the end of the film!
        foreach( OMfilmAxisParams* axis, params->axes) {
            // scale movement times with new time difference
            if( axis->endTm != 0 )
                axis->endTm = (float) axis->endTm * timeDiff;

            if( axis->decelTm != 0 )
                axis->decelTm = (float) axis->decelTm * timeDiff;

            if( axis->startTm != 0 )
                axis->startTm = (float) axis->startTm * timeDiff;

            if( axis->accelTm != 0 )
                axis->accelTm = (float) axis->accelTm * timeDiff;

            if( axis->endTm > params->realLength )
                axis->endTm = 0;
            if( axis->startTm > params->realLength )
                axis->startTm = 0;
        }

     }

    m_params->releaseParams();

        // if we updated the film time based on real time changes..
    if( showUpdate )
        _showFilmTime();
    else if( camEn && autoFPS ) {
        qDebug() << "FW: Force Time Constraints";
        _checkFilmTimeConstraint(); // update time constraints
    }
    else if( camEn && ! autoFPS )
        _calcAutoFilmTime(); // update film time




}

 // apply constraints to film length, and update display as needed
void FilmWindow::_checkFilmTimeConstraint() {

     qDebug() << "FW: Checking Time Constraints";

        // TODO: Add UI indicator that limitation has
        // been placed...

    OMfilmParams* params = m_params->getParams();

    unsigned long minInt = m_exec->minInterval(params);

    unsigned long maxFrames = params->realLength / minInt;
    unsigned long maxTime   = (maxFrames / params->fps) * 1000;

    maxTime = maxTime < 1000 ? 1000 : maxTime;

    qDebug() << "FW: TimeConstraint: MT=" << maxTime << "LN=" << params->length;

    if( params->length > maxTime ) {
        params->length = maxTime;
        m_params->releaseParams();
        _showFilmTime();
        qDebug() << "FW: Constraining maximum film time";
    }
    else {
        m_params->releaseParams(false);
    }

}

void FilmWindow::on_filmHHSpin_valueChanged(unsigned int p_val) {
    if( ! m_ignoreUpdate ) {
        qDebug() << "FW: on_filmHHSpin_valueChanged";
        _changeTime(1, 1, p_val);
        _checkFilmTimeConstraint();
    }
}

void FilmWindow::on_filmMMSpin_valueChanged(unsigned int p_val) {
    if( ! m_ignoreUpdate ) {
        qDebug() << "FW: on_filmMMSpin_valueChanged";
        _changeTime(1, 2, p_val);
        _checkFilmTimeConstraint();
    }
}

void FilmWindow::on_filmSSSpin_valueChanged(unsigned int p_val) {
    if( ! m_ignoreUpdate ) {
        qDebug() << "FW: on_filmSSSpin_valueChanged";
        _changeTime(1, 3, p_val);
        _checkFilmTimeConstraint();
    }
}

void FilmWindow::on_realHHSpin_valueChanged(unsigned int p_val) {
    if( ! m_ignoreUpdate ) {
        _changeTime(2, 1, p_val);
    }
}

void FilmWindow::on_realMMSpin_valueChanged(unsigned int p_val) {
    if( ! m_ignoreUpdate ) {
        _changeTime(2, 2, p_val);
    }
}

void FilmWindow::on_realSSSpin_valueChanged(unsigned int p_val) {
    if( ! m_ignoreUpdate ) {
        _changeTime(2, 3, p_val);
    }
}

void FilmWindow::_calcAutoFilmTime() {

    qDebug() << "FW: Calc Auto Film Time";

    OMfilmParams* params = m_params->getParams();
    unsigned long interval = m_exec->interval(params);

    unsigned short fps = params->fps;
    unsigned long filmTm = params->length;
    unsigned long wallTm = params->realLength;


        // calculate # of shots
    unsigned long shots = wallTm / interval;

        // determine output film time in mS
    filmTm = TimeConverter::msFromSeconds((float)shots / (float)fps);

        // store film length
    params->length = filmTm;
    m_params->releaseParams();

        // update display
    _showFilmTime();
    qDebug() << "FW: New Film Time: " << filmTm;

    return;


}

 /*

   Shuttle Controls

   */

void FilmWindow::on_playButton_clicked() {

    OMfilmParams* params = m_params->getParams();
    int mode = params->filmMode;
    m_params->releaseParams();

    int bstat = 0;
    int fstat = m_exec->status();

    m_error = false;

    if( fstat != FILM_STARTED ) {
        qDebug() << "FW: Send Start";

        m_busy->setLabelText("Sending All Axes Home");
        m_busy->setMinimum(0);
        m_busy->setMaximum(0);
        m_busy->show();

        m_exec->start();
        bstat = 1;
    }
    else if( mode == FILM_MODE_SMS && fstat == FILM_STARTED ) {
        qDebug() << "FW: Send Pause";
        m_exec->pause();
        bstat = 2;
    }

    if( bstat == 1 ) {
        if( mode == FILM_MODE_SMS )
            _setPlayButtonStatus(s_Pause);
        else
            _setPlayButtonStatus(s_DisPres);
    }
    else if( bstat == 2 ) {
        _setPlayButtonStatus(s_Play);
    }

    _setStopButtonStatus(s_Enable);

    qDebug() << "FW: Play: Checking error" << m_error;

        // did an error occur during starting?
    if( m_error )
        on_stopButton_clicked();
}

void FilmWindow::on_stopButton_clicked() {

    _setStopButtonStatus(s_Disable);
    _setPlayButtonStatus(s_Play);

    qDebug() << "FW: Sending Stop";

    m_exec->stop();
    _inputEnable(true);
    emit playStatusChange(false);

}

void FilmWindow::on_rewindButton_clicked() {
    qDebug() << "FW: Send Rewind";
    _inputEnable(false);

    m_busy->setLabelText("Sending All Axes to Start Point");
    m_busy->setMinimum(0);
    m_busy->setMaximum(0);
    m_busy->show();

    m_exec->rewind();

}

void FilmWindow::on_forwardButton_clicked() {
    qDebug() << "FW: Send Forward";
    _inputEnable(false);

    m_busy->setLabelText("Sending All Axes to End Point");
    m_busy->setMinimum(0);
    m_busy->setMaximum(0);
    m_busy->show();

    m_exec->ffwd();

}

void FilmWindow::_setPlayButtonStatus(int p_stat) {

    if( p_stat == s_DisPres ) {
        ui->playButton->setEnabled(false);
        ui->playButton->setDown(true);
    }
    if( p_stat == s_Disable ) {
        ui->playButton->setEnabled(false);
    }
    else {
        ui->playButton->setEnabled(true);
    }


   if( p_stat == s_Play ) {
       ui->playButton->setDown(false);
       ui->playButton->setState(0);
        // need to update stylesheet as it reads a custom property for border-image
        // selection
       ui->playButton->style()->unpolish(ui->playButton);
       ui->playButton->style()->polish(ui->playButton);
       this->update();
    }
    else if( p_stat == s_Pause ) {
       ui->playButton->setDown(false);
       ui->playButton->setState(1);
       ui->playButton->style()->unpolish(ui->playButton);
       ui->playButton->style()->polish(ui->playButton);
       this->update();
    }
}

void FilmWindow::_setStopButtonStatus(int p_stat) {
    if( p_stat == s_Disable )
        ui->stopButton->setEnabled(false);
    else
        ui->stopButton->setEnabled(true);
}




void FilmWindow::_playStatus(bool p_stat, unsigned long p_time) {

   static bool wasStat = false;

        // not running anymore, turn off our status timers and such
    if( p_stat == false && wasStat == true && m_exec->status() != FILM_PAUSED ) {
        unsigned long len = m_params->getParams()->realLength;
        m_params->releaseParams(false);
        _filmTimeDisplay(len);
        on_stopButton_clicked();
    }
    else
        _filmTimeDisplay(p_time);

    wasStat = p_stat;
}

 // update time displays as status data comes back from FilmExec

void FilmWindow::_filmTimeDisplay(unsigned long p_ms) {

    int rhh = TimeConverter::hours(p_ms);
    int rmm = TimeConverter::freeMinutes(p_ms);
    int rss = TimeConverter::freeSeconds(p_ms);

    _popTimeDisplay(ui->runHHLabel, rhh);
    _popTimeDisplay(ui->runMMLabel, rmm);
    _popTimeDisplay(ui->runSSLabel, rss);

    OMfilmParams* parms = m_params->getParams();
    float timeDiv = (float) parms->length / (float) parms->realLength;

    if( ! parms->camParams->camControl ) {

        _popTimeDisplay(ui->filmHHLabel, rhh);
        _popTimeDisplay(ui->filmMMLabel, rmm);
        _popTimeDisplay(ui->filmSSLabel, rss);

        ui->curFrameDispLabel->setText(QString::number((int) ((float) p_ms * ((float) parms->fps / 1000.0))));
        ui->totalFrameDispLabel->setText(QString::number((int) (parms->realLength / 1000) * parms->fps));

    }
    else {
        unsigned long filmTm = ( (float) p_ms * timeDiv);

        _popTimeDisplay(ui->filmHHLabel, (int)TimeConverter::hours(filmTm));
        _popTimeDisplay(ui->filmMMLabel, (int)TimeConverter::freeMinutes(filmTm));
        _popTimeDisplay(ui->filmSSLabel, (int)TimeConverter::freeSeconds(filmTm));

        unsigned long interval = m_exec->interval(parms);
        ui->curFrameDispLabel->setText(QString::number((int) (p_ms / interval)));
        ui->totalFrameDispLabel->setText(QString::number((int) (parms->realLength / interval) ));
    }

    m_params->releaseParams(false);

}

void FilmWindow::_popTimeDisplay(QLabel *p_label, int p_time) {
    QString disp;

    if( p_time < 10 )
        disp = "0" + QString::number(p_time);
    else
        disp = QString::number(p_time);

    p_label->setText(disp);
}

void FilmWindow::_redrawMotionOverlay() {
    m_areaViewPort->removeEventFilter(m_filter);

    delete m_motion;
    m_motion = new MotionSection(m_exec, m_params, m_areaViewPort);
    m_motion->show();

    delete m_filter;
    m_filter = new SectionResizeFilter(m_motion, this);
    m_areaViewPort->installEventFilter(m_filter);

    connect(this, SIGNAL(motionAreaBorders(int,int)), m_motion, SLOT(setBorders(int,int)));

}


void FilmWindow::_filmStarted() {
    qDebug() << "FW: Got filmStarted";
    m_busy->hide();
    _inputEnable(false);
    emit playStatusChange(true);
}


/** Error Handling Slot */
void FilmWindow::error(QString p_err) {

    if( ! m_error ) {
        m_error = true;

            // make sure everything is stopped (may trigger another error)
        on_stopButton_clicked();

        m_busy->hide();

        ErrorDialog error;
        error.setError(p_err);
        error.exec();
    }

}

 // What to do when the busy window (e.g. sending nodes home) is cancelled?
void FilmWindow::_busyCanceled() {
    qDebug() << "FW: Busy Canceled";
    on_stopButton_clicked();
}


 // Change the enable state of several inputs,
 // as is required when playing/stopping the film

void FilmWindow::_inputEnable(bool p_stat) {
    ui->realHHSpin->setEnabled(p_stat);
    ui->realMMSpin->setEnabled(p_stat);
    ui->realSSSpin->setEnabled(p_stat);
    ui->forwardButton->setEnabled(p_stat);
    ui->rewindButton->setEnabled(p_stat);
    ui->camControlSlider->setEnabled(p_stat);

        // only control status of certain camera inputs, if
        // cam control enabled

    OMfilmParams params = m_params->getParamsCopy();
    bool cam = params.camParams->camControl;
    bool autoFPS = params.camParams->autoFPS;
    bool mode = params.filmMode;

    if( cam ) {
        ui->camSetBut->setEnabled(p_stat);
        if( autoFPS ) {
            ui->filmHHSpin->setEnabled(p_stat);
            ui->filmMMSpin->setEnabled(p_stat);
            ui->filmSSSpin->setEnabled(p_stat);
        }

        if( mode == FILM_MODE_SMS ) {
            ui->frameFwdButton->setEnabled(p_stat);
            ui->frameRwdButton->setEnabled(p_stat);
        }
        else {
            ui->frameFwdButton->setEnabled(false);
            ui->frameRwdButton->setEnabled(false);
        }
    }
    else {
        ui->camSetBut->setEnabled(false);
        ui->frameFwdButton->setEnabled(false);
        ui->frameRwdButton->setEnabled(false);
    }
}

void FilmWindow::on_plugJogButton_clicked() {
    ui->pluginStackedWidget->setCurrentWidget(m_jcp);
   // ui->plugJogButton->setDown(true);
}

/** Trigger Film File Load Slot

  Triggers a load file dialog
  */

void FilmWindow::load() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Film"), "", tr("Film Files (*.film)"));
    qDebug() << "FW: FilmOpen Got File: " << fileName;
    FilmFileHandler::readFile(fileName, m_params, m_net);
}

/** Trigger Film File Save Slot

  Triggers a save file dialog
  */

void FilmWindow::save() {

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Film"), "", tr("Film Files (*.film)"));
    qDebug() << "FW: FilmSave Got File: " << fileName;
    FilmFileHandler::writeFile(fileName, m_params);
}

void FilmWindow::filmParamsChanged() {
        // re-display inputs when params change
    _prepInputs();
}


void FilmWindow::_shuttleComplete() {
    m_busy->hide();
    _inputEnable(true);
}
