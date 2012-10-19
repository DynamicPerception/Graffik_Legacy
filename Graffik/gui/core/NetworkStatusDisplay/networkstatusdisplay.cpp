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

#include "networkstatusdisplay.h"
#include "ui_networkstatusdisplay.h"

NetworkStatusDisplay::NetworkStatusDisplay(OMMoCoBus* c_bus, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NetworkStatusDisplay)
{
    ui->setupUi(this);

    m_bus = c_bus;
 //   m_busInfo = m_bus->getBusInfo();
    this->_updateDisplay();
}

NetworkStatusDisplay::~NetworkStatusDisplay()
{
    delete ui;
}

void NetworkStatusDisplay::_updateDisplay() {
 /*   ui->nameLabel->setText(m_busInfo->name);
    ui->portLabel->setText(m_busInfo->port);
    ui->commandLCD->display((int) m_busInfo->commands);
    ui->deviceLCD->display((int) m_busInfo->devices);
    ui->errorLCD->display((int) m_busInfo->errors);
    this->repaint();*/
}

