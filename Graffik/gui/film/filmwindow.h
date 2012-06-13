#ifndef FILMWINDOW_H
#define FILMWINDOW_H

#include <QWidget>


#include "MoCoBus/omnetwork.h"
#include "film/LiveDeviceModel/livedevicemodel.h"
#include "core/AxisOptions/axisoptions.h"
#include "film/JogControlManager/jogcontrolmanager.h"

namespace Ui {
class filmWindow;
}

class filmWindow : public QWidget
{
    Q_OBJECT
    
public:
    filmWindow(OMNetwork* c_net, AxisOptions* c_opts, QWidget *parent = 0);
    ~filmWindow();

private slots:
    void _jogMotorChangeDenied(unsigned short p_oldAddr);

private:
    Ui::filmWindow *ui;
    LiveDeviceModel* m_ldModel;
    OMNetwork* m_net;
    AxisOptions* m_opts;
    JogControlManager* m_jcm;

    bool m_threadStarted;


};

#endif // FILMWINDOW_H
