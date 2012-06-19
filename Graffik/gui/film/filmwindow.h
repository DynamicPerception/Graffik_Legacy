#ifndef FILMWINDOW_H
#define FILMWINDOW_H

#include <QWidget>
#include <QHash>
#include <QVBoxLayout>


#include "MoCoBus/omnetwork.h"
#include "core/AxisOptions/axisoptions.h"
#include "film/JogControlManager/jogcontrolmanager.h"
#include "film/LiveDeviceModel/livedevicemodel.h"
#include "film/MotionArea/motionbase.h"
#include "film/FilmParameters/filmparameters.h"

namespace Ui {
    class FilmWindow;
}

class FilmWindow : public QWidget
{
    Q_OBJECT
    
public:
    FilmWindow(OMNetwork* c_net, AxisOptions* c_opts, QWidget *parent = 0);
    ~FilmWindow();

private slots:
    void _jogMotorChangeDenied(unsigned short p_oldAddr);
    void _drawNewAxis(OMdeviceInfo* p_dev);
    void _eraseAxis(QString p_bus, unsigned short p_addr);

private:
    Ui::FilmWindow *ui;
    LiveDeviceModel* m_ldModel;
    OMNetwork* m_net;
    AxisOptions* m_opts;
    JogControlManager* m_jcm;
    FilmParameters* m_params;

    QVBoxLayout* m_areaLayout;

    bool m_threadStarted;

    QHash<unsigned short, MotionBase*> m_areaBlocks;


};

#endif // FILMWINDOW_H
