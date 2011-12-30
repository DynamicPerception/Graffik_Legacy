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

