#include "cameracontroldialog.h"
#include "ui_cameracontroldialog.h"

#include <QDebug>

CameraControlDialog::CameraControlDialog(FilmParameters *c_params, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CameraControlDialog)
{
    ui->setupUi(this);

    m_params = c_params;

    _setupInputs();

}

CameraControlDialog::~CameraControlDialog()
{
    delete ui;
}

void CameraControlDialog::on_bulbCheck_stateChanged(int p_state) {
    if( p_state == Qt::Unchecked )
        ui->bulbSpin->setEnabled(false);
    else
        ui->bulbSpin->setEnabled(true);
}

void CameraControlDialog::on_focusCheck_stateChanged(int p_state) {
    if( p_state == Qt::Unchecked )
        ui->focusSpin->setEnabled(false);
    else
        ui->focusSpin->setEnabled(true);
}

void CameraControlDialog::on_intervalCheck_stateChanged(int p_state) {
    bool state = true;

    if( p_state == Qt::Unchecked )
        state = false;

    ui->hhSpinBox->setEnabled(state);
    ui->mmSpinBox->setEnabled(state);
    ui->ssSpinBox->setEnabled(state);

        // fps is only used if manual intervalometer isn't set
    ui->fpsSpin->setEnabled(! state);
}

void CameraControlDialog::on_autoCheck_stateChanged(int p_state) {
    bool state = false;

    if( p_state == Qt::Unchecked )
        state = true;

    ui->intervalCheck->setEnabled(state);

    if( state == true && ui->intervalCheck->isChecked() ) {
        ui->hhSpinBox->setEnabled(true);
        ui->mmSpinBox->setEnabled(true);
        ui->ssSpinBox->setEnabled(true);
    }
    else {
        ui->hhSpinBox->setEnabled(false);
        ui->mmSpinBox->setEnabled(false);
        ui->ssSpinBox->setEnabled(false);
    }

    if( state == false || ui->intervalCheck->isChecked() )
        ui->fpsSpin->setEnabled(false);
    else if( state == true && ! ui->intervalCheck->isChecked() )
        ui->fpsSpin->setEnabled(true);

}

void CameraControlDialog::_setupInputs() {

    OMfilmParams params = m_params->getParamsCopy();

    OMfilmCamParams* cam = params.camParams;

    bool bulb = cam->bulb;
    bool foc  = cam->focus;
    bool lck  = cam->focLock;
    bool man  = cam->manInterval;
    bool aFPS = cam->autoFPS;

    bool sms  = params.filmMode == FILM_MODE_SMS ? true : false;


    ui->autoCheck->setChecked(aFPS);

    bool enManControls = aFPS ? false : man ? true : false;
    bool enFPS = enManControls ? false : aFPS ? false : true;

        // set interval inputs
    unsigned long ihh = cam->interval / 60 / 60;
    unsigned long imm = (cam->interval - (ihh * 60 * 60)) / 60;
    unsigned long iss = cam->interval - (ihh * 60 * 60) - (imm * 60);

    ui->intervalCheck->setChecked(enManControls);
    ui->hhSpinBox->setEnabled(enManControls);
    ui->hhSpinBox->setValue(ihh);
    ui->hhSpinBox->setMaximum(999);

    ui->mmSpinBox->setEnabled(enManControls);
    ui->mmSpinBox->setValue(imm);
    ui->mmSpinBox->setMaximum(59);

    ui->ssSpinBox->setEnabled(enManControls);
    ui->ssSpinBox->setValue(iss);
    ui->ssSpinBox->setMaximum(59);

    // one can either set the interval, or set the FPs for the film
    // can't do both

    ui->fpsSpin->setValue( params.fps );
    ui->fpsSpin->setEnabled( enFPS );

    // set other inputs

    ui->bulbSpin->setMaximum(CAM_MAX_EXPOSURE);
    ui->focusSpin->setMaximum(CAM_MAX_FOCUS);
    ui->delaySpin->setMaximum(CAM_MAX_DELAY);

    ui->bulbCheck->setChecked(bulb);
    ui->bulbSpin->setEnabled(bulb);
    ui->bulbSpin->setValue( (float) cam->shutterMS / 1000.0 );

    ui->focusCheck->setChecked(foc);
    ui->focusSpin->setEnabled(foc);
    ui->focusSpin->setValue( (float) cam->focusMS / 1000.0 );

    ui->lockCheck->setChecked(lck);
    ui->smsCheck->setChecked(sms);

    ui->delaySpin->setValue( (float) cam->delayMS / 1000.0 );


}

void CameraControlDialog::accept() {

    OMfilmParams* parmRef = m_params->getParams();

    OMfilmCamParams* cam = parmRef->camParams;

    cam->bulb = ui->bulbCheck->isChecked();
    cam->focus = ui->focusCheck->isChecked();
    cam->focLock = ui->lockCheck->isChecked();
    cam->autoFPS = ui->autoCheck->isChecked();

    if( cam->bulb )
        cam->shutterMS = ui->bulbSpin->value() * 1000.0;
    else
        cam->shutterMS = CAM_DEF_EXPOSURE;


    if( cam->focus )
        cam->focusMS = ui->focusSpin->value() * 1000.0;
    else
        cam->focusMS = CAM_DEF_FOCUS;

    cam->delayMS = ui->delaySpin->value() * 1000.0;

    parmRef->filmMode = ui->smsCheck->isChecked() == true ? FILM_MODE_SMS : FILM_MODE_CONT;


    cam->manInterval = ui->intervalCheck->isChecked();

        // set new interval time, if manual interval selected
    if( cam->manInterval ) {
        unsigned long iv = ui->ssSpinBox->value();
        iv += ui->mmSpinBox->value() * 60;
        iv += ui->hhSpinBox->value() * 60 * 60;
        cam->interval = iv;

        qDebug() << "CCD: Interval: " << iv;
    }


    parmRef->fps = ui->fpsSpin->value();

    m_params->releaseParams();

    this->done(1);

}
