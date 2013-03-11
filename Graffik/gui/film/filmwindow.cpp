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

    m_net   = c_net;
    m_opts  = c_opts;
    m_gopts = c_gopts;

    m_error        = false;
    m_spinsPrepped = false;
    m_ignoreUpdate = false;
    m_checkRunning = false;

    m_params = new FilmParameters(m_net, this);
    m_exec   = new FilmExec(m_net, m_params, m_opts, m_gopts, this);
    m_busy   = new QProgressDialog(this);
    m_jcp    = new JogControlPanel(m_net, m_opts, m_params, this);
    m_notw   = new NoTracksWidget(this);
    m_time   = new FilmTimeManager(m_exec, m_params);
    m_saver  = new FilmAutoSaver(m_net, m_params, this);

    m_areaLayout = new QVBoxLayout;
    m_areaLayout->setContentsMargins(0, 0, 0, 0);
    m_areaLayout->setSpacing(0);
    m_areaLayout->addWidget(m_notw);

    m_notwShown = true;

    m_areaViewPort = new QWidget;
    m_areaViewPort->setLayout(m_areaLayout);

    ui->visualSA->setWidget(m_areaViewPort);

    m_curFrameShot = 0;

    // OSX has issues laying out these buttons w/o overlap,
    // this is a work-around
#ifdef Q_WS_MAC
    ui->stopButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);
    ui->playButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);
    ui->forwardButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);
    ui->rewindButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);
    ui->frameFwdButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);
    ui->frameRwdButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);
#endif

        // create our transparent overlay for drawing position line
        // this must be done after the layout is added to visualSAContents,
        // as this class attempts to access that layout

    m_motion = new MotionSection(m_params, m_areaViewPort);
    m_filter = new SectionResizeFilter(m_motion, this);

        // need to capture resize events to resize our transparent overlay
    m_areaViewPort->installEventFilter(m_filter);

    m_motion->show();


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

    connect(this, SIGNAL(playStatusChange(bool)), m_jcp, SIGNAL(playStatusChange(bool)));
    connect(this, SIGNAL(playStatusChange(bool)), m_tape, SLOT(disableClicks(bool)));
    connect(this, SIGNAL(playStatusChange(bool)), m_saver, SLOT(playStatusChanged(bool)));

    connect(m_tape, SIGNAL(timelineClick(ulong)), this, SLOT(timelineClicked(ulong)));

        // let these guys monitor time position changes
    connect(m_time, SIGNAL(timePositionChanged(ulong)), m_tape, SLOT(timeChanged(ulong)));
    connect(m_time, SIGNAL(timePositionChanged(ulong)), m_motion, SLOT(timeChanged(ulong)));

    connect(m_jcp, SIGNAL(emergencyStop()), this, SLOT(_emergencyStop()));

    _prepInputs();

    // theming

    Themer* theme = &Singleton<Themer>::Instance();
    connect(theme, SIGNAL(themeChanged()), this, SLOT(_themeChanged()));

    setStyleSheet(theme->getThemeCSS("film"));

        // start out at beginning of film!
    m_time->timePosition(0);

}

FilmWindow::~FilmWindow() {

    foreach(unsigned short addr, m_areaBlocks.keys()) {
        delete m_areaBlocks.value(addr);
        m_areaBlocks.remove(addr);
    }

    delete m_saver;
    delete m_time;
    delete m_tape;
    delete m_areaLayout;
    delete m_motion;
    delete m_filter;

    delete m_exec;
    delete m_jcp;
    delete m_params;
    delete m_areaViewPort;
    delete ui;
}


 /** Perform activities that must be done after complete program initialization */

void FilmWindow::postInitialize() {
        // load any saved film
    m_saver->loadSavedFilm();

        // recover film state
    bool filmState = m_saver->savedStatus();

    if( filmState == true ) {
        // state was previously running, let's see if it still is and if we
        // can restore to a previous point
        m_checkRunning = true;
        m_exec->checkRunning();
    }
}

void FilmWindow::_themeChanged() {
    setStyleSheet(SingleThemer::getStyleSheet("film"));
    style()->unpolish(this);
    style()->polish(this);
    update();
}


void FilmWindow::_emergencyStop() {
    qDebug() << "FW: Got Emergency Stop Signal!";
    m_exec->stop();
}

/** Time Line Clicked Slot

  Handles receiving the timelineClicked signal from MotionTape

  @param p_time
  The time clicked by the user
  */

void FilmWindow::timelineClicked(unsigned long p_time) {

    int filmXpos = 0;

    QHash<unsigned short, long> theseAxes;

    foreach( unsigned short addr, m_areaBlocks.keys() ) {
        MotionBase*             mb = m_areaBlocks.value(addr);
        MotionPathPainter* painter = mb->area()->getPathPainter();

        if( filmXpos == 0 )
            filmXpos = painter->getX(p_time);

        float position = painter->getPosition(filmXpos);

        qDebug() << "FW: timeLine: Position for" << addr << position;

        theseAxes.insert(addr, position);
    }

    qDebug() << "FW: Send Nodes to Position";
    _inputEnable(false);

    m_error = false;

    m_busy->setLabelText("Sending All Axes to Specified Point");
    m_busy->setMinimum(0);
    m_busy->setMaximum(0);
    m_busy->show();

    m_exec->sendAxesTo(theseAxes);

    m_time->timePosition(p_time);
}

void FilmWindow::_drawNewAxis(OMdeviceInfo *p_dev) {

        // don't show a motion area for device types that
        // do not support it

    if( p_dev->type != "OpenMoCo Axis")
        return;

        // hide the no track shown widget if currently shown
    if( m_notwShown == true ) {
        m_areaLayout->removeWidget(m_notw);
        m_notwShown = false;
    }

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

            // if there are no more tracks, show our warning area
        if( m_areaBlocks.count() < 1 ) {
            m_areaLayout->addWidget(m_notw);
            m_notwShown = true;
        }

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
    CameraControlDialog* control = new CameraControlDialog(m_params, m_net, m_opts);
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

    _setStopButtonStatus(s_Disable);
    _setPlayButtonStatus(s_Play);

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

    bool           autoFPS = params->camParams->autoFPS;
    bool             camEn = params->camParams->camControl;
    unsigned long  realLen = params->realLength;
    unsigned long      fps = params->fps;
    unsigned long interval = m_exec->interval(params);
    bool        showUpdate = false;

    if( p_which == 1 ) {
        params->length = mS;
    }
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

    if( ! camEn )
        _showTotalFrames(realLen, fps);
    else
        _showTotalFrames(realLen, interval, true);
}

 // apply constraints to film length, and update display as needed
void FilmWindow::_checkFilmTimeConstraint() {

     qDebug() << "FW: Checking Time Constraints";

        // TODO: Add UI indicator that limitation has
        // been placed (if one is placed)...

     // before we go around accessing film params, let's clear out
     // and SMS track errors that might've been set before...

     // this can cause a problem when a track has both SMS errors
     // AND normal motion curve errors (unlikely, considering the
     // issues for continuous motion should not be possible with
     // SMS moves and vice-versa, but keep an eye out for the future

    foreach(unsigned short addr, m_smsErrorTracks)
         m_areaBlocks.value(addr)->area()->moveSane(true);

    m_smsErrorTracks.clear();


    OMfilmParams*    params = m_params->getParams();
    unsigned long    minInt = m_exec->minInterval(params);
    unsigned long maxFrames = params->realLength / minInt;
    unsigned long   maxTime = (maxFrames / params->fps) * 1000;

        // for SMS moves, we have to check each track and see
        // if it can actually achieve the desired movement in the
        // specified real time.
    if( params->filmMode == FILM_MODE_SMS ) {
        m_smsErrorTracks = _checkSMSMovements(params);
    }


    maxTime = maxTime < 1000 ? 1000 : maxTime;

    qDebug() << "FW: TimeConstraint: MT=" << maxTime << "LN=" << params->length;

    if( params->length > maxTime ) {
        qDebug() << "FW: Constraining maximum film time";
        params->length = maxTime;
        m_params->releaseParams();
        _showFilmTime();
    }
    else {
        m_params->releaseParams(false);
    }

        // now, do any track muting we need to...
    foreach(unsigned short addr, m_smsErrorTracks)
        m_areaBlocks.value(addr)->area()->moveSane(false);


}

// Check SMS movements for sanity

QList<unsigned short> FilmWindow::_checkSMSMovements(OMfilmParams *p_params) {

    QHash<unsigned short, OMfilmAxisParams*> axes = p_params->axes;
    QList<unsigned short> muteTracks;

    unsigned long     interval = m_exec->interval(p_params);
    unsigned long       minInt = m_exec->minInterval(p_params);
    unsigned long freeInterval = interval - minInt;
    unsigned long   filmLength = p_params->realLength;


    foreach(unsigned short addr, axes.keys() ) {

        qDebug() << "FW: Checking SMS Sanity for" << addr;

        OMfilmAxisParams* axParms = axes.value(addr);

                 long    moveDist = axParms->endDist;
        unsigned long   startShot = axParms->startTm / interval;
        unsigned long     endShot = axParms->endTm > 0 ? axParms->endTm / interval : filmLength / interval;
        unsigned long travelShots = endShot - startShot;
        unsigned long    maxSpeed = m_opts->getOptions(addr)->maxSteps;
        float             maxMove = m_areaBlocks.value(addr)->area()->getPathPainter()->getMaxSpeed();


            // set move as not sane if it can't be fulfilled

        m_opts->removeError(addr, AxisErrors::ErrorNoInterval);
        m_opts->removeError(addr, AxisErrors::ErrorNoTime);
        m_opts->removeError(addr, AxisErrors::ErrorIntervalSpeed);

            // normalize number to positive integer
        if( moveDist < 0)
            moveDist *= -1;

        if( moveDist > 0 ) {

            bool fail = false;

            if( freeInterval < 1 ) {
                qDebug() << "FW: SMS Sane: No free interval time" << addr;
                m_opts->error(addr, AxisErrors::ErrorNoInterval);
                fail = true;
            }
            else if( ((freeInterval * travelShots) / 1000) < (moveDist / maxSpeed) ) {
                qDebug() << "FW: SMS Sane: Not enough time to move all steps" << addr << moveDist << freeInterval  << travelShots;
                m_opts->error(addr, AxisErrors::ErrorNoTime);
                fail = true;
            }
            else if( (freeInterval / 1000) < (maxMove / maxSpeed) ) {
                qDebug() << "FW: SMS Sane: Max Move will cause interval to be exceeded" << addr << maxMove;
                m_opts->error(addr, AxisErrors::ErrorIntervalSpeed);
                fail = true;
            }

            if( fail ) {
                    // we can't actually call moveSane() here, because the chain of signals
                    // and slots will result in a call to ask for the filmParams.
                muteTracks.append(addr);
            }
        } // end movedist > 0
    } // end foreach

    return muteTracks;
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
        _checkFilmTimeConstraint();
    }
}

void FilmWindow::on_realMMSpin_valueChanged(unsigned int p_val) {
    if( ! m_ignoreUpdate ) {
        _changeTime(2, 2, p_val);
        _checkFilmTimeConstraint();
    }
}

void FilmWindow::on_realSSSpin_valueChanged(unsigned int p_val) {
    if( ! m_ignoreUpdate ) {
        _changeTime(2, 3, p_val);
        _checkFilmTimeConstraint();
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
    m_params->releaseParams(false);

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

        m_curFrameShot = 0;

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

    m_curFrameShot = 0;

    m_exec->stop();
    _inputEnable(true);
    emit playStatusChange(false);

}

void FilmWindow::on_rewindButton_clicked() {
    qDebug() << "FW: Send Rewind";
    _inputEnable(false);

    m_error = false;

    m_busy->setLabelText("Sending All Axes to Start Point");
    m_busy->setMinimum(0);
    m_busy->setMaximum(0);
    m_busy->show();

    m_exec->rewind();

    m_curFrameShot = 0;

    m_time->timePosition(0);

}

void FilmWindow::on_forwardButton_clicked() {
    qDebug() << "FW: Send Forward";
    _inputEnable(false);

    m_error = false;

    m_busy->setLabelText("Sending All Axes to End Point");
    m_busy->setMinimum(0);
    m_busy->setMaximum(0);
    m_busy->show();

    m_exec->ffwd();

    m_curFrameShot = 0;

    OMfilmParams*     params = m_params->getParams();
    unsigned long realLength = params->realLength;

    m_params->releaseParams(false);

    m_time->timePosition(realLength);
}

void FilmWindow::on_frameFwdButton_clicked() {

    m_error = false;

    OMfilmParams*     params = m_params->getParams();
    unsigned long   interval = m_exec->interval(params);
    unsigned long totalShots = params->realLength / interval;

    m_params->releaseParams(false);

    if( m_curFrameShot >= totalShots )
        return;

    m_curFrameShot++;

        // advance one frame, and set correct position of time
        // indicator
    m_exec->frameAdvance();
    m_time->timePosition(m_curFrameShot * interval);
}

void FilmWindow::on_frameRwdButton_clicked() {
    m_error = false;

    OMfilmParams*   params = m_params->getParams();
    unsigned long interval = m_exec->interval(params);

    m_params->releaseParams(false);

        // reverse frame, if we're at a frame north of zero
        // and set correct location of visual time indicator
    if( m_curFrameShot > 0 ) {
        m_exec->frameReverse();
        m_curFrameShot--;
        m_time->timePosition(m_curFrameShot * interval);
    }
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

        // did we issue a request to check running status?
   if( m_checkRunning == true ) {
       m_checkRunning = false;

       if( p_stat == true ) {
           // set correct button states!

           OMfilmParams* parms = m_params->getParams();
           int mode = parms->filmMode;
           m_params->releaseParams(false);

           if( mode == FILM_MODE_SMS )
               _setPlayButtonStatus(s_Pause);
           else
               _setPlayButtonStatus(s_DisPres);

           _setStopButtonStatus(s_Enable);
           emit playStatusChange(true);
       }

       m_time->timePosition(p_time);

   }
   else if( p_stat == false && wasStat == true && m_exec->status() != FILM_PAUSED ) {
               // not running anymore, turn off our status timers and such
        unsigned long len = m_params->getParams()->realLength;
        m_params->releaseParams(false);
        _filmTimeDisplay(len);
        on_stopButton_clicked();
    }
    else {
        _filmTimeDisplay(p_time);
   }

    wasStat = p_stat;
}


 // update the total frames display

void FilmWindow::_showTotalFrames(unsigned long p_len, unsigned long p_val, bool p_type) {
    if( ! p_type )
        ui->totalFrameDispLabel->setText(QString::number((int) (p_len / 1000) * p_val));
    else
        ui->totalFrameDispLabel->setText(QString::number((int) p_len / p_val));
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
        _showTotalFrames(parms->realLength, parms->fps);

    }
    else {
        unsigned long filmTm = ( (float) p_ms * timeDiv);

        _popTimeDisplay(ui->filmHHLabel, (int)TimeConverter::hours(filmTm));
        _popTimeDisplay(ui->filmMMLabel, (int)TimeConverter::freeMinutes(filmTm));
        _popTimeDisplay(ui->filmSSLabel, (int)TimeConverter::freeSeconds(filmTm));

        unsigned long interval = m_exec->interval(parms);
        ui->curFrameDispLabel->setText(QString::number((int) (p_ms / interval)));
        _showTotalFrames(parms->realLength, interval, true);
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

    disconnect(m_time, SIGNAL(timePositionChanged(ulong)), m_motion, SLOT(timeChanged(ulong)));

    delete m_motion;
    m_motion = new MotionSection(m_params, m_areaViewPort);
    m_motion->show();

    delete m_filter;
    m_filter = new SectionResizeFilter(m_motion, this);
    m_areaViewPort->installEventFilter(m_filter);

    connect(this, SIGNAL(motionAreaBorders(int,int)), m_motion, SLOT(setBorders(int,int)));
    connect(m_time, SIGNAL(timePositionChanged(ulong)), m_motion, SLOT(timeChanged(ulong)));

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
    qDebug() << "FW: Got Film Params Changed";
    _checkFilmTimeConstraint();
    _prepInputs();
}


void FilmWindow::_shuttleComplete() {
    m_busy->hide();
    _inputEnable(true);
}
