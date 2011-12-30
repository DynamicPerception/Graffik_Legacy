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
