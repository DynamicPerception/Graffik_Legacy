#ifndef FILMWINDOW_H
#define FILMWINDOW_H

#include <QWidget>

#include "MoCoBus/omnetwork.h"
#include "film/LiveDeviceModel/livedevicemodel.h"
#include "film/SpeedControlProxy/speedcontrolproxy.h"

namespace Ui {
class filmWindow;
}

class filmWindow : public QWidget
{
    Q_OBJECT
    
public:
    filmWindow(OMNetwork* c_net, QWidget *parent = 0);
    ~filmWindow();
    
private:
    Ui::filmWindow *ui;
    LiveDeviceModel* _ldModel;
    SpeedControlProxy* _scp;
    OMNetwork* _net;
};

#endif // FILMWINDOW_H
