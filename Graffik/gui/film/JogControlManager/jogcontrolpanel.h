#ifndef JOGCONTROLPANEL_H
#define JOGCONTROLPANEL_H

#include <QWidget>

#include "jogcontrolmanager.h"
#include "livedevicemodel.h"

#include "core/Options/axisoptions.h"
#include "film/FilmParameters/filmparameters.h"
#include "MoCoBus/omnetwork.h"

namespace Ui {
class JogControlPanel;
}

class JogControlPanel : public QWidget
{
    Q_OBJECT
    
public:
    JogControlPanel(OMNetwork* c_net, AxisOptions* c_opts, FilmParameters* c_params, QWidget *parent = 0);
    ~JogControlPanel();
    
private slots:
    void _endSet(unsigned short p_addr, long p_dist);
    void _jogMotorChangeDenied(unsigned short p_oldAddr);

private:
    Ui::JogControlPanel *ui;

    OMNetwork* m_net;
    AxisOptions* m_opts;
    FilmParameters* m_params;
    JogControlManager* m_jcm;
    LiveDeviceModel* m_ldModel;
};

#endif // JOGCONTROLPANEL_H
