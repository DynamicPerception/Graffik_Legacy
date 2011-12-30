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
