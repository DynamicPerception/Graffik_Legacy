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

#ifndef NETWORKSTATUSDISPLAY_H
#define NETWORKSTATUSDISPLAY_H

#include <QWidget>
#include <QString>

#include "MoCoBus/ommocobus.h"
#include "MoCoBus/omnetwork.h"

namespace Ui {
class NetworkStatusDisplay;
}

class NetworkStatusDisplay : public QWidget
{
    Q_OBJECT
    
public:
    explicit NetworkStatusDisplay(OMMoCoBus*, QWidget *parent = 0);
    ~NetworkStatusDisplay();
    
private:
    Ui::NetworkStatusDisplay *ui;

    OMMoCoBus* m_bus;
    OMbusInfo* m_busInfo;

    void _updateDisplay();

};

#endif // NETWORKSTATUSDISPLAY_H
