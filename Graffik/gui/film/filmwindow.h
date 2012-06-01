#ifndef FILMWINDOW_H
#define FILMWINDOW_H

#include <QWidget>


#include "MoCoBus/omnetwork.h"
#include "film/LiveDeviceModel/livedevicemodel.h"
#include "film/OMAxisFilmOptions/omaxisfilmoptions.h"
#include "film/JogControlManager/jogcontrolmanager.h"

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
    void _jogMotorChangeDenied(unsigned short p_oldAddr);

private:
    Ui::filmWindow *ui;
    LiveDeviceModel* m_ldModel;
    OMNetwork* m_net;
    OMAxisFilmOptions* m_opts;
    JogControlManager* m_jcm;

    bool m_threadStarted;


};

#endif // FILMWINDOW_H
