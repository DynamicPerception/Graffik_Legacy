#ifndef FILMWINDOW_H
#define FILMWINDOW_H

#include <QWidget>
#include <QThread>

#include "MoCoBus/omnetwork.h"
#include "film/LiveDeviceModel/livedevicemodel.h"
#include "film/SpeedControlProxy/speedcontrolproxy.h"
#include "film/OMAxisFilmOptions/omaxisfilmoptions.h"

namespace Ui {
class filmWindow;
}

class filmWindow : public QWidget
{
    Q_OBJECT
    
public:
    filmWindow(OMNetwork* c_net, OMAxisFilmOptions* c_opts, QWidget *parent = 0);
    ~filmWindow();
    
private slots:
    void _liveDeviceSelected(unsigned short p_addr);
    void _jogMaxSpeedChange(double p_spd);
    void _jogDampChange(double p_damp);

private:
    Ui::filmWindow *ui;
    LiveDeviceModel* _ldModel;
    SpeedControlProxy* _scp;
    OMNetwork* _net;
    OMAxisFilmOptions* _opts;
    QThread* _scpThread;

    bool m_threadStarted;
    unsigned short m_curAxis;

    double _stepsToJogSpeed(OMaxisOptions* p_opts, unsigned int p_steps);
    unsigned int _jogSpeedToSteps(OMaxisOptions* p_opts, double p_speed);

    void _prepJogInputs(unsigned short p_addr);

};

#endif // FILMWINDOW_H
