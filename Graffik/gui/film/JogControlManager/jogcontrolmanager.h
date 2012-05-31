#ifndef JOGCONTROLMANAGER_H
#define JOGCONTROLMANAGER_H

#include <QObject>
#include <QDoubleSpinBox>
#include <QThread>
#include <QDial>
#include <QComboBox>

#include "speedcontrolproxy.h"
#include "MoCoBus/omnetwork.h"
#include "film/OMAxisFilmOptions/omaxisfilmoptions.h"
#include "film/LiveDeviceModel/livedevicemodel.h"

#define OM_JOGRES_RAPID 1
#define OM_JOGRES_COARSE 2
#define OM_JOGRES_FINE 8
#define OM_JOGRES_XFINE 16

/** Jog Controls Manager

  This class manages the interactions with the Jog Controls on the
  film screen.

  */

class JogControlManager : public QObject
{
    Q_OBJECT
public:
    JogControlManager(OMNetwork* c_net, OMAxisFilmOptions* c_opts, LiveDeviceModel* c_ldm, QComboBox* c_jogCombo, QDial* c_jogDial, QDoubleSpinBox* c_jogSpd, QDoubleSpinBox* c_jogDmp, QObject *parent = 0);
    ~JogControlManager();

signals:
        // if the SCP denies a chaneg to motor (usually because a damp is still in progress)
        // this signal will be emitted, indicating which motor is -still- selected
    void motorChangeDenied(unsigned short p_keepaddr);

public slots:
    
private slots:
    void _liveDeviceSelected(unsigned short p_addr);
    void _jogMaxSpeedChange(double p_spd);
    void _jogDampChange(double p_damp);
    void _jogResChange(int p_idx);

private:
    unsigned short m_curAxis;
    unsigned int m_curRes;

    QDoubleSpinBox* m_jogSpd;
    QDoubleSpinBox* m_jogDmp;
    QDial* m_jogDial;
    QComboBox* m_jogCombo;

    OMNetwork* m_net;
    OMAxisFilmOptions* m_opts;
    LiveDeviceModel* m_ldm;
    SpeedControlProxy* m_scp;

    QThread* m_scpThread;

    double _stepsToJogSpeed(OMaxisOptions* p_opts, unsigned int p_steps);
    unsigned int _jogSpeedToSteps(OMaxisOptions* p_opts, double p_speed);
    void _prepJogInputs(unsigned short p_addr);

};

#endif // JOGCONTROLMANAGER_H
