#include "cameracontroldialog.h"
#include "ui_cameracontroldialog.h"

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

void CameraControlDialog::_setupInputs() {

    OMfilmParams params = m_params->getParamsCopy();

    OMfilmCamParams* cam = params.camParams;

    bool bulb = cam->bulb;
    bool foc  = cam->focus;
    bool lck  = cam->focLock;

    bool sms  = params.filmMode == FILM_MODE_SMS ? true : false;

    ui->bulbCheck->setChecked(bulb);
    ui->bulbSpin->setEnabled(bulb);
    ui->bulbSpin->setValue( cam->shutterMS / 1000 );

    ui->focusCheck->setChecked(foc);
    ui->focusSpin->setEnabled(foc);
    ui->focusSpin->setValue( cam->focusMS / 1000 );

    ui->lockCheck->setChecked(lck);
    ui->smsCheck->setChecked(sms);


}

