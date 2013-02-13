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

#include "addressnotfounddialog.h"
#include "ui_addressnotfounddialog.h"


#include <QDebug>

AddressNotFoundDialog::AddressNotFoundDialog(unsigned short c_addr, QHash<unsigned short, QString> c_list, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddressNotFoundDialog)
{
    ui->setupUi(this);
    m_replace = c_list;

    setWindowModality(Qt::WindowModal);

    foreach(QString name, m_replace.values()) {
        ui->devCombo->addItem(name);
    }

    ui->addressLabel->setText(QString::number(c_addr));

}

AddressNotFoundDialog::~AddressNotFoundDialog()
{
    delete ui;
}

void AddressNotFoundDialog::accept() {

    QString selectedName = ui->devCombo->currentText();
    int selAddr = 1;

    qDebug() << "ANFD: Selected new device: " << selectedName;

    foreach( unsigned short addr, m_replace.keys() ) {
        if( m_replace.value(addr) == selectedName )
            selAddr = addr;
    }

    qDebug() << "ANFD: New device address:" << selAddr;

   // setResult(selAddr);
   // QDialog::accept();
    QDialog::done(selAddr);

    qDebug() << "ANFD: Done done";

}
