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

#ifndef NETWORKSTATUSGRID_H
#define NETWORKSTATUSGRID_H

#include <QGridLayout>
#include "networkstatusdisplay.h"

class NetworkStatusGrid : public QGridLayout
{

 Q_OBJECT

 public:
    explicit NetworkStatusGrid(QWidget *parent = 0);

    void addNet(NetworkStatusDisplay*);

 private:

};


#endif // NETWORKSTATUSGRID_H

