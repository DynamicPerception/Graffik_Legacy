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

#include "devicescanwidget.h"
#include "ui_devicescanwidget.h"

DeviceScanWidget::DeviceScanWidget(QWidget *parent) : QWidget(parent), ui(new Ui::DeviceScanWidget) {
    ui->setupUi(this);
    ui->doneButton->hide();

    setStyleSheet(SingleThemer::getStyleSheet("device_scan"));
}

DeviceScanWidget::~DeviceScanWidget() {
    delete ui;
}

void DeviceScanWidget::enableConfirm(bool p_enable) {
    if( p_enable ) {
        ui->doneButton->setEnabled(true);
        ui->doneButton->setDefault(true);
    }
    else {
        ui->doneButton->setEnabled(false);
    }
}

void DeviceScanWidget::addNote(QString p_note) {
    ui->statusText->append(p_note);
}


void DeviceScanWidget::totalNodes(int p_nodes) {
    ui->progressBar->setMaximum(p_nodes);
    ui->progressBar->setMinimum(0);
}

void DeviceScanWidget::scannedNodes(int p_nodes) {
    ui->progressBar->setValue(p_nodes);
}


void DeviceScanWidget::nodesFound(bool p_found) {
    if( p_found == true ) {
        ui->doneButton->setText("Next");
        ui->doneButton->show();
    }
    else
        ui->doneButton->hide();
}

/*void DeviceScanWidget::on_cancelButton_clicked() {
    emit rejected();
}*/

void DeviceScanWidget::on_doneButton_clicked() {
   emit accepted();
}
