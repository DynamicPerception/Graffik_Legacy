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

#include "networkstatusgrid.h"

NetworkStatusGrid::NetworkStatusGrid(QWidget *parent) {

}

void NetworkStatusGrid::addNet(NetworkStatusDisplay * p_net) {
    int rowCnt = this->rowCount();
    int row = rowCnt;
    int col = 0;

        // we've already zoomed in on the current row,
        // but see if there's free space in the second
        // column here, if not, move until we get to a
        // free row+col combination

    while( this->itemAtPosition(row, col) != 0 ) {
        if( col < 1 ) {
            col++;
        }
        else {
            col = 0;
            row++;
        }

    }

    this->addWidget(p_net, row, col, 1, 1);
}
