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

#include <QList>
#include <QVariant>

#include <QDebug>

#include "addnetdialog.h"
#include "ui_addnetdialog.h"

#include "qextserialenumerator.h"

#include "core/Dialogs/errordialog.h"

AddNetDialog::AddNetDialog(OMNetwork *c_net, QWidget *c_parent) : QDialog(c_parent), ui(new Ui::AddNetDialog) {
    ui->setupUi(this);

    _parent = c_parent;
    _net = c_net;

    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    updateSerialPorts();

    QWidget::setTabOrder(ui->portCombo, ui->netName);
}

AddNetDialog::~AddNetDialog() {

    delete ui;
}


void AddNetDialog::updateSerialPorts() {
    QList<QextPortInfo> ports = QextSerialEnumerator::getPorts();
    QList<QString> usedPorts = _net->getBuses();

        // clear out the combo box
    while( ui->portCombo->count() > 0 ) {
        ui->portCombo->removeItem( ui->portCombo->count() - 1 );
    }

    foreach( QextPortInfo port, ports ) {
        // do not add port to select if it is already in use
        if( usedPorts.contains(port.portName) ) {
            qDebug() << "Already have used port " << port.portName;
            continue;
        }

        ui->portCombo->addItem(port.portName, QVariant(port.friendName));
    }

    ui->portHintLabel->setText(ports[0].friendName);
}

void AddNetDialog::on_rescanButton_clicked() {
    this->updateSerialPorts();
}

void AddNetDialog::on_portCombo_currentIndexChanged(int p_idx) {
    ui->portHintLabel->setText(ui->portCombo->itemData(p_idx).toString());

}


void AddNetDialog::on_okButton_clicked() {
    QString port = ui->portCombo->itemText(ui->portCombo->currentIndex());
    QString name = ui->netName->text();

    bool ok = true;
    QString errString = "";

    if( name.isEmpty() ) {
        errString = "You must enter a name for this bus.";
        ok = false;
    }

    if( ok == true ) {
        try {
            _net->addBus(port, name);
        }
        catch (int e) {
            ok = false;
            if( e == OM_NET_DUPE ) {
                errString = "The communication port selected is already in-use";
            }
            else {
                errString = "An unknown error occured, please try again. Error Code: " + QString::number(e);
            }
        }
    }


    if( ok == false ) {
        // throw out error dialog
        ErrorDialog er(this);
        er.setError(errString);
        er.exec();
    }
    else {

        this->done(QDialog::Accepted);
    }

}

void AddNetDialog::on_cancelButton_clicked() {
    this->done(QDialog::Rejected);
}
