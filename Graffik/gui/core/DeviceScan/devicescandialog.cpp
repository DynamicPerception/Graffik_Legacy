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

#include "devicescandialog.h"
#include "ui_devicescandialog.h"

DeviceScanDialog::DeviceScanDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeviceScanDialog)
{
    ui->setupUi(this);
    ui->doneButton->setText("Done");
}

DeviceScanDialog::~DeviceScanDialog()
{
    delete ui;
}

void DeviceScanDialog::enableConfirm(bool p_enable) {
    if( p_enable ) {
        ui->doneButton->setEnabled(true);
        ui->cancelButton->setDefault(false);
        ui->doneButton->setDefault(true);
    }
    else {
        ui->doneButton->setEnabled(false);
    }
}

void DeviceScanDialog::addNote(QString p_note) {
    ui->statusText->append(p_note);
}


void DeviceScanDialog::totalNodes(int p_nodes) {
    ui->progressBar->setMaximum(p_nodes);
    ui->progressBar->setMinimum(0);
}

void DeviceScanDialog::scannedNodes(int p_nodes) {
    ui->progressBar->setValue(p_nodes);
}


void DeviceScanDialog::nodesFound(bool p_found) {
    if( p_found == true )
        ui->doneButton->setText("Next");
    else
        ui->doneButton->setText("Done");
}

void DeviceScanDialog::on_cancelButton_clicked() {
    reject();
}

void DeviceScanDialog::on_doneButton_clicked() {
    accept();
}
