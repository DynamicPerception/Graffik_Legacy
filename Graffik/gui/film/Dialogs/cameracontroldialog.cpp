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

void CameraControlDialog::on_bulbExpToggle_selected(int p_val) {
    if( p_val == 1 )
        ui->bulbSpin->setEnabled(true);
    else
        ui->bulbSpin->setEnabled(false);
}

void CameraControlDialog::on_focusConToggle_selected(int p_val) {
    if( p_val == 1 )
        ui->focusSpin->setEnabled(true);
    else
        ui->focusSpin->setEnabled(false);
}

void CameraControlDialog::on_manIntToggle_selected(int p_val) {
    bool state = true;

    if( p_val == 0 )
        state = false;

    ui->hhSpinBox->setEnabled(state);
    ui->mmSpinBox->setEnabled(state);
    ui->ssSpinBox->setEnabled(state);

}

void CameraControlDialog::on_filmLenToggle_selected(int p_val) {
    bool state = false;

    if( p_val == 0 )
        state = true;

    ui->manIntToggle->setEnabled(state);

    if( state == true && ui->manIntToggle->value() == 1 ) {
        ui->hhSpinBox->setEnabled(state);
        ui->mmSpinBox->setEnabled(state);
        ui->ssSpinBox->setEnabled(state);
    }
    else {
        ui->hhSpinBox->setEnabled(false);
        ui->mmSpinBox->setEnabled(false);
        ui->ssSpinBox->setEnabled(false);
    }

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


    ui->filmLenToggle->setValue(aFPS);

    bool enManControls = aFPS ? false : man ? true : false;
    // bool enFPS = enManControls ? false : aFPS ? false : true;

        // set interval inputs
    unsigned long ihh = TimeConverter::hours(cam->interval);
    unsigned long imm = TimeConverter::freeMinutes(cam->interval);
    unsigned long iss = TimeConverter::freeSeconds(cam->interval);

    ui->manIntToggle->setValue(enManControls);
    ui->hhSpinBox->setEnabled(enManControls);
    ui->hhSpinBox->setCount(99);
    ui->hhSpinBox->setValue(ihh);

    ui->mmSpinBox->setEnabled(enManControls);
    ui->mmSpinBox->setCount(59);
    ui->mmSpinBox->setValue(imm);

    ui->ssSpinBox->setEnabled(enManControls);
    ui->ssSpinBox->setCount(59);
    ui->ssSpinBox->setValue(iss);

    ui->fpsSpin->setValue( params.fps );


    // set other inputs

    ui->bulbSpin->setMaximum(CAM_MAX_EXPOSURE);
    ui->focusSpin->setMaximum(CAM_MAX_FOCUS);
    ui->delaySpin->setMaximum(CAM_MAX_DELAY);

    ui->bulbExpToggle->setValue(bulb);
    ui->bulbSpin->setEnabled(bulb);
    ui->bulbSpin->setValue( (float) TimeConverter::seconds(cam->shutterMS) );

    ui->focusConToggle->setValue(foc);
    ui->focusSpin->setEnabled(foc);
    ui->focusSpin->setValue( (float) TimeConverter::seconds(cam->focusMS) );

    ui->focLocToggle->setValue(lck);
    ui->smsToggle->setValue(sms);

    ui->delaySpin->setValue( (float) TimeConverter::seconds(cam->delayMS) );


}

void CameraControlDialog::accept() {

    OMfilmParams* parmRef = m_params->getParams();

    OMfilmCamParams* cam = parmRef->camParams;

    cam->bulb = ui->bulbExpToggle->value();
    cam->focus = ui->focusConToggle->value();
    cam->focLock = ui->focLocToggle->value();
    cam->autoFPS = ui->filmLenToggle->value();

    if( cam->bulb )
        cam->shutterMS = TimeConverter::msFromSeconds(ui->bulbSpin->value());
    else
        cam->shutterMS = CAM_DEF_EXPOSURE;


    if( cam->focus )
        cam->focusMS = TimeConverter::msFromSeconds(ui->focusSpin->value());
    else
        cam->focusMS = CAM_DEF_FOCUS;

    cam->delayMS = TimeConverter::msFromSeconds(ui->delaySpin->value());

    parmRef->filmMode = ui->smsToggle->value() == 1 ? FILM_MODE_SMS : FILM_MODE_CONT;


    cam->manInterval = ui->manIntToggle->value();

        // set new interval time, if manual interval selected
    if( cam->manInterval ) {
        unsigned long iv = ui->ssSpinBox->value();
        iv += ui->mmSpinBox->value() * 60;
        iv += ui->hhSpinBox->value() * 60 * 60;

        iv = TimeConverter::msFromSeconds(iv); // convert back to mS
        cam->interval = iv;


        qDebug() << "CCD: Interval: " << iv;
    }


    parmRef->fps = ui->fpsSpin->value();

    m_params->releaseParams();

    this->done(1);

}
