#ifndef FILMWINDOW_H
#define FILMWINDOW_H

#include <QWidget>

#include "film/LiveDeviceModel/livedevicemodel.h"
#include "MoCoBus/omnetwork.h"

namespace Ui {
class filmWindow;
}

class filmWindow : public QWidget
{
    Q_OBJECT
    
public:
    explicit filmWindow(OMNetwork* c_net, QWidget *parent = 0);
    ~filmWindow();
    
private:
    Ui::filmWindow *ui;
    LiveDeviceModel* _ldModel;
    OMNetwork* _net;
};

#endif // FILMWINDOW_H
