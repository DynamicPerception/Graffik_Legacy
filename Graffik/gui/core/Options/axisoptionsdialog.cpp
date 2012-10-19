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

#include <QDebug>

#include "axisoptionsdialog.h"
#include "ui_axisoptionsdialog.h"

#include "core/Dialogs/confirmdialog.h"

AxisOptionsDialog::AxisOptionsDialog(AxisOptions* c_opts, unsigned short c_addr, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AxisOptionsDialog)
{
    ui->setupUi(this);
    m_opts = c_opts->getOptions(c_addr);
    m_optObj = c_opts;
    m_addr = c_addr;

    _initInputs();

        // do NOT use Qt::QueuedConnection here - you want a direct connection on the accept signal, otherwise
        // this object may be gone by the time the signal is handled.

    QObject::connect(ui->defaultCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(_defaultComboChange(int)));
    QObject::connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(_accept()));

}

AxisOptionsDialog::~AxisOptionsDialog()
{
    delete ui;
}


void AxisOptionsDialog::_initInputs() {

        // populate "defaults" combo box
    ui->defaultCombo->addItem(AOD_STR_PAN, AXIS_VX1_PAN);
    ui->defaultCombo->addItem(AOD_STR_TILT, AXIS_VX1_TILT);
    ui->defaultCombo->addItem(AOD_STR_SLIDE, AXIS_SX1_SLIDE);
    ui->defaultCombo->addItem(AOD_STR_CUST, AXIS_CUSTOM);

        // get current "default" type for this axis
    int combShowSel = ui->defaultCombo->findData(m_opts->axisType);
    ui->defaultCombo->setCurrentIndex(combShowSel);

    ui->typeCombo->addItem(AOD_STR_ROT, AXIS_MOVE_ROT);
    ui->typeCombo->addItem(AOD_STR_LIN, AXIS_MOVE_LIN);

    // get current "default" type for this axis
    combShowSel = ui->typeCombo->findData(m_opts->axisMove);
    ui->typeCombo->setCurrentIndex(combShowSel);

    ui->ratioLine->setText(QString::number(m_opts->ratio));
    ui->maxLine->setText(QString::number(m_opts->maxSteps));

    ui->masterCheck->setChecked(m_opts->master);

        // disable inputs if one of the pre-defined types
    if( m_opts->axisType != AXIS_CUSTOM ) {
        ui->maxLine->setDisabled(true);
        ui->ratioLine->setDisabled(true);
        ui->typeCombo->setDisabled(true);

    }

}

    // when the selection is changed on the default combo box..

void AxisOptionsDialog::_defaultComboChange(int p_idx) {

    bool enable = false;
    unsigned int sel = ui->defaultCombo->itemData(p_idx).toUInt();

    switch(sel) {
        case AXIS_CUSTOM:
            enable = true;
            break;
        case AXIS_VX1_PAN:
            // pass down...
        case AXIS_VX1_TILT:
            enable = false;
            ui->ratioLine->setText(QString::number(OM_OPT_VX1RATIO));
            ui->maxLine->setText(QString::number(OM_OPT_VX1MAXSTEPS));
            ui->typeCombo->setCurrentIndex(ui->typeCombo->findData(AXIS_MOVE_ROT));
            break;
        case AXIS_SX1_SLIDE:
            enable = false;
            ui->ratioLine->setText(QString::number(OM_OPT_SX1RATIO));
            ui->maxLine->setText(QString::number(OM_OPT_SX1MAXSTEPS));
            ui->typeCombo->setCurrentIndex(ui->typeCombo->findData(AXIS_MOVE_LIN));
            break;
    }

        // pre-defined types cannot be customized
    if( enable == false ) {
        ui->maxLine->setDisabled(true);
        ui->ratioLine->setDisabled(true);
        ui->typeCombo->setDisabled(true);
    }
    else {
        ui->maxLine->setDisabled(false);
        ui->ratioLine->setDisabled(false);
        ui->typeCombo->setDisabled(false);
    }

}

    // slot connected to accept signal from buttonbox
    // save dialog values

void AxisOptionsDialog::_accept() {

    qDebug() << "AFODia: User Saved Values";

    m_opts->axisType = ui->defaultCombo->itemData(ui->defaultCombo->currentIndex()).toUInt();
    m_opts->axisMove = ui->typeCombo->itemData(ui->typeCombo->currentIndex()).toUInt();
    m_opts->ratio = ui->ratioLine->text().toFloat();
    m_opts->maxSteps = ui->maxLine->text().toUInt();

    m_opts->jogLimit = m_opts->maxSteps;
    m_opts->jogDamp = OM_OPT_JOGDAMP;

    if( m_opts->master != ui->masterCheck->isChecked() ) {
            // changing the timing master flag has implications, force the user to confirm
            // do nothing if they do not confirm
        if( m_opts->master == false ) {
            ConfirmDialog dia(AOD_STR_MAST, this);
            int res = dia.exec();
            if( res == QDialog::Accepted ) {
                m_optObj->setMaster(m_addr);
            }
        }
        else {
            ConfirmDialog dia(AOD_STR_UNMAST, this);
            int res = dia.exec();
            if( res == QDialog::Accepted ) {
                m_opts->master = false;
            }

        }
    }

        // this may seem redundant since we're working with a pointer to the actual options,
        // but it forces the axis options object to do any necessary cleanup and signaling
        // after we muck with the contents

    m_optObj->setOptions(m_addr, m_opts);

}
