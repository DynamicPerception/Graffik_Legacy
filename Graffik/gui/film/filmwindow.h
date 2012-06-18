#ifndef FILMWINDOW_H
#define FILMWINDOW_H

#include <QWidget>
#include <QHash>
#include <QVBoxLayout>


#include "MoCoBus/omnetwork.h"
#include "film/LiveDeviceModel/livedevicemodel.h"
#include "core/AxisOptions/axisoptions.h"
#include "film/JogControlManager/jogcontrolmanager.h"
#include "core/MotionArea/motionarea.h"

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
    void _drawNewAxis(OMdeviceInfo* p_dev);
    void _eraseAxis(QString p_bus, unsigned short p_addr);

private:
    Ui::filmWindow *ui;
    LiveDeviceModel* m_ldModel;
    OMNetwork* m_net;
    AxisOptions* m_opts;
    JogControlManager* m_jcm;

    QVBoxLayout* m_areaLayout;

    bool m_threadStarted;

    QHash<unsigned short, MotionArea*> m_areaBlocks;


};

#endif // FILMWINDOW_H
