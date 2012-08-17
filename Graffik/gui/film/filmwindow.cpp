#include "filmwindow.h"
#include "ui_filmwindow.h"

#include <QDebug>


FilmWindow::FilmWindow(OMNetwork* c_net, AxisOptions *c_opts, QWidget *parent) : QWidget(parent), ui(new Ui::FilmWindow) {

    ui->setupUi(this);

    m_net = c_net;
    m_opts = c_opts;

    m_error = false;

    m_ldModel = new LiveDeviceModel(m_net, this);
    m_jcm = new JogControlManager(m_net, m_opts, m_ldModel, ui->jogResCombo, ui->jogDial, ui->jogSpeedSpin, ui->jogDampSpin, ui->jogHomeButton, ui->jogEndButton, this);
    m_params = new FilmParameters(m_net, this);
    m_exec = new FilmExec(m_net, m_params, m_opts);
    m_busy = new QProgressDialog(this);

        // connect the device list display to the live device model
    ui->devButtonList->setModel(m_ldModel);


    m_areaLayout = new QVBoxLayout;
    ui->visualSAContents->setLayout(m_areaLayout);

        // create our transparent overlay for drawing position line
        // this must be done after the layout is added to visualSAContents,
        // as this class attempts to access that layout

    m_motion = new MotionSection(m_exec, m_params, ui->visualSAContents);
    m_filter = new SectionResizeFilter(m_motion, this);

    m_motion->show();

        // need to capture resize events to resize our transparent overlay
    ui->visualSAContents->installEventFilter(m_filter);

    m_tape = new MotionTape(m_params, ui->visualSAContents, this);
    ui->tapeVLayout->addWidget(m_tape);



        // we need to populate motion area displays
    connect(m_net, SIGNAL(deviceAdded(OMdeviceInfo*)), this, SLOT(_drawNewAxis(OMdeviceInfo*)));
    connect(m_net, SIGNAL(deviceDeleted(QString,unsigned short)), this, SLOT(_eraseAxis(QString,unsigned short)));

        // pass a click on to the model via signal
    connect(ui->devButtonList, SIGNAL(clicked(const QModelIndex &)), m_ldModel, SLOT(deviceClicked(const QModelIndex &)));

    connect(m_jcm, SIGNAL(motorChangeDenied(unsigned short)), this, SLOT(_jogMotorChangeDenied(unsigned short)));
    connect(m_jcm, SIGNAL(endPosition(unsigned short,long)), this, SLOT(_endSet(unsigned short,long)));


    connect(m_exec, SIGNAL(filmPlayStatus(bool,ulong)), this, SLOT(_playStatus(bool,ulong)));
    connect(m_exec, SIGNAL(filmStarted()), this, SLOT(_filmStarted()));
    connect(m_exec, SIGNAL(error(QString)), this, SLOT(error(QString)));


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

    delete m_exec;
    delete m_params;
    delete m_jcm;
    delete m_ldModel;
    delete ui;
}

void FilmWindow::_jogMotorChangeDenied(unsigned short p_oldAddr) {
    qDebug() << "FW: Motor change was denied, re-setting devButtonList selection" << p_oldAddr;

    int row = m_ldModel->find(p_oldAddr);

    qDebug() << "FW: New Row =" << row;

    QModelIndex tl = m_ldModel->index(row, 0, QModelIndex());
    QModelIndex tr = m_ldModel->index(row, 0, QModelIndex());
    QItemSelection sel(tl, tr);
    ui->devButtonList->selectionModel()->select(sel, QItemSelectionModel::SelectCurrent);
}

void FilmWindow::_drawNewAxis(OMdeviceInfo *p_dev) {

        // don't show a motion area for device types that
        // do not support it

    if( p_dev->type != "OpenMoCo Axis")
        return;

    qDebug() << "FW: Got new axis, spawning motion area" << p_dev->type;

    MotionBase* area = new MotionBase(m_params, p_dev, m_opts, this);
    m_areaBlocks.insert(p_dev->device->address(), area);
    m_areaLayout->addWidget(area);

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

void FilmWindow::on_camControlCheckBox_stateChanged(int p_state) {

    bool camControl = false;

    if( p_state == Qt::Checked )
        camControl = true;

    qDebug() << "FW: Cam Control Set to: " << camControl;

    _enableCamControl(camControl);

}

void FilmWindow::_enableCamControl(bool p_en) {

    OMfilmParams* params = m_params->getParams();
    params->camParams->camControl = p_en;
    bool autoFPS = params->camParams->autoFPS;
    m_params->releaseParams();

    ui->camSetBut->setEnabled(p_en);

        // disable film time spinners unless auto fps is enabled
    if( autoFPS && p_en )
        p_en = true;
    else
        p_en = false;

    ui->filmHHSpin->setEnabled(p_en);
    ui->filmMMSpin->setEnabled(p_en);
    ui->filmSSSpin->setEnabled(p_en);



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
    else
        en = true;

    qDebug() << "FW: Auto Time: " << autoFPS << en;

    ui->filmHHSpin->setEnabled(en);
    ui->filmMMSpin->setEnabled(en);
    ui->filmSSSpin->setEnabled(en);



    delete control;
}

void FilmWindow::_showFilmTime() {
    OMfilmParams* params = m_params->getParams();
    unsigned long wallTM = params->realLength;
    unsigned long filmTM = params->length;
    m_params->releaseParams(false);

        // convert from mS to hours, minutes, and seconds

    unsigned long whh = wallTM / 1000 / 60 / 60;
    unsigned long wmm = (wallTM - (whh * 1000 * 60 * 60)) / 1000 / 60;
    unsigned long wss = (wallTM - (whh * 1000 * 60 * 60) - (wmm * 1000 * 60)) / 1000;

    unsigned long fhh = filmTM / 1000 / 60 / 60;
    unsigned long fmm = (filmTM - (fhh * 1000 * 60 * 60)) / 1000 / 60;
    unsigned long fss = (filmTM - (fhh * 1000 * 60 * 60) - (fmm * 1000 * 60)) / 1000;

    ui->filmHHSpin->setValue(fhh);
    ui->filmMMSpin->setValue(fmm);
    ui->filmSSSpin->setValue(fss);

    ui->realHHSpin->setValue(whh);
    ui->realMMSpin->setValue(wmm);
    ui->realSSSpin->setValue(wss);

}

void FilmWindow::_prepInputs() {
        // default is no camera control
    _enableCamControl(false);

    _showFilmTime();

    ui->filmHHSpin->setMaximum(999);
    ui->filmMMSpin->setMaximum(59);
    ui->filmSSSpin->setMaximum(59);

    ui->realHHSpin->setMaximum(999);
    ui->realMMSpin->setMaximum(59);
    ui->realSSSpin->setMaximum(59);

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
    }

    if( p_pos == 1 )
        hh = p_val;
    else if( p_pos == 2 )
        mm = p_val;
    else
        ss = p_val;

    unsigned long mS = (hh * 60 * 60 * 1000) + (mm * 60 * 1000) + (ss * 1000);

    OMfilmParams* params = m_params->getParams();

    if( p_which == 1 )
        params->length = mS;
    else {
        params->realLength = mS;

            // can't have axis moves end beyond the end of the film!

        foreach( OMfilmAxisParams* axis, params->axes) {
            if( axis->endTm > params->realLength )
                axis->endTm = 0;
            if( axis->startTm > params->realLength )
                axis->startTm = 0;
        }
     }

    m_params->releaseParams();

}

 // apply constraints to film length, and update display as needed
void FilmWindow::_checkFilmTimeConstraint() {

        // TODO: Calculate maximum film time when auto FPS is used
        // instead of intervalometer

        // TODO: Add UI indicator that limitation has
        // been placed...

    OMfilmParams* params = m_params->getParams();

    if( params->length > params->realLength ) {
        params->length = params->realLength;
        m_params->releaseParams();
        _showFilmTime();
        qDebug() << "FW: Constraining maximum film time to real time";
    }
    else {
        m_params->releaseParams(false);
    }

}

void FilmWindow::on_filmHHSpin_valueChanged(int p_val) {
    _changeTime(1, 1, p_val);
    _checkFilmTimeConstraint();
}

void FilmWindow::on_filmMMSpin_valueChanged(int p_val) {
    _changeTime(1, 2, p_val);
    _checkFilmTimeConstraint();}

void FilmWindow::on_filmSSSpin_valueChanged(int p_val) {
    _changeTime(1, 3, p_val);
    _checkFilmTimeConstraint();
}

void FilmWindow::on_realHHSpin_valueChanged(int p_val) {
    _changeTime(2, 1, p_val);
}

void FilmWindow::on_realMMSpin_valueChanged(int p_val) {
    _changeTime(2, 2, p_val);
}

void FilmWindow::on_realSSSpin_valueChanged(int p_val) {
    _changeTime(2, 3, p_val);
}

void FilmWindow::_calcAutoFilmTime() {

    OMfilmParams* params = m_params->getParams();
    unsigned long interval = m_exec->interval(params);

    unsigned short fps = params->fps;
    unsigned long filmTm = params->length;
    unsigned long wallTm = params->realLength;


        // calculate # of shots
    unsigned long shots = wallTm / interval;

        // determine output film time in mS
    filmTm = (shots / fps) * 1000;

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
            _setPlayButtonStatus(s_Disable);
    }
    else if( bstat == 2 ) {
        _setPlayButtonStatus(s_Play);
    }

    _setStopButtonStatus(s_Enable);
}

void FilmWindow::on_stopButton_clicked() {

    m_error = false;

    int fstat = m_exec->status();

    if( fstat == FILM_STOPPED )
        return;

    qDebug() << "FW: Sending Stop";

    m_exec->stop();

    _setStopButtonStatus(s_Disable);
    _setPlayButtonStatus(s_Play);

}

void FilmWindow::_setPlayButtonStatus(int p_stat) {
    if( p_stat == s_Disable ) {
        ui->playButton->setEnabled(false);
        return;
    }
    else {
        ui->playButton->setEnabled(true);
    }

    if( p_stat == s_Play ) {
        ui->playButton->setText("Play");
    }
    else if( p_stat == s_Pause ) {
        ui->playButton->setText("Pause");
    }
}

void FilmWindow::_setStopButtonStatus(int p_stat) {
    if( p_stat == s_Disable )
        ui->stopButton->setEnabled(false);
    else
        ui->stopButton->setEnabled(true);
}


void FilmWindow::_endSet(unsigned short p_addr, long p_dist) {
    qDebug() << "FW: Got EndSet Signal" << p_addr << p_dist;

    OMfilmParams* parms = m_params->getParams();

    parms->axes.value(p_addr)->endDist = p_dist;

    m_params->releaseParams();
}


void FilmWindow::_playStatus(bool p_stat, unsigned long p_time) {

   static bool wasStat = false;

        // not running anymore, turn off our status timers and such
    if( p_stat == false && wasStat == true ) {
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

    int rhh = p_ms / 1000 / 60 / 60;
    int rmm = (p_ms - (rhh * 1000 * 60 * 60)) / 1000 / 60;
    int rss = (p_ms - (rhh * 1000 * 60 * 60) - (rmm * 1000 * 60)) / 1000;

    ui->runHHLCD->display(rhh);
    ui->runMMLCD->display(rmm);
    ui->runSSLCD->display(rss);

    OMfilmParams* parms = m_params->getParams();
    float timeDiv = (float) parms->length / (float) parms->realLength;

    if( ! parms->camParams->camControl ) {
        ui->filmHHLCD->display(rhh);
        ui->filmMMLCD->display(rmm);
        ui->filmSSLCD->display(rss);

        ui->curFrameLCD->display((int) (p_ms / 1000) * parms->fps);
        ui->totFrameLCD->display((int) (parms->realLength / 1000) * parms->fps);

    }
    else {
        unsigned long filmTm = ( (float) p_ms * timeDiv);
        int fhh = filmTm / 1000 / 60 / 60;
        int fmm = (filmTm - (fhh * 1000 * 60 * 60)) / 1000 / 60;
        int fss = (filmTm - (fhh * 1000 * 60 * 60) - (fmm * 1000 * 60)) / 1000;
        ui->filmHHLCD->display(fhh);
        ui->filmMMLCD->display(fmm);
        ui->filmSSLCD->display(fss);

        unsigned long interval = m_exec->interval(parms);
        ui->curFrameLCD->display((int) (p_ms / interval));
        ui->totFrameLCD->display((int) (parms->realLength / interval) );
    }

    m_params->releaseParams(false);

}


void FilmWindow::_redrawMotionOverlay() {
    ui->visualSAContents->removeEventFilter(m_filter);
    delete m_motion;
    m_motion = new MotionSection(m_exec, m_params, ui->visualSAContents);
    m_motion->show();
    delete m_filter;
    m_filter = new SectionResizeFilter(m_motion, this);
    ui->visualSAContents->installEventFilter(m_filter);
}

void FilmWindow::_filmStarted() {
    qDebug() << "FW: Got filmStarted";
    m_busy->hide();
}

void FilmWindow::error(QString p_err) {

    if( ! m_error ) {
        m_error = true;

        m_busy->hide();

        ErrorDialog error;
        error.setError(p_err);
        error.exec();

            // make sure everything is stopped (may trigger another error)
        on_stopButton_clicked();
    }

}
