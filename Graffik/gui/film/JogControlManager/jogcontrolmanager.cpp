#include "jogcontrolmanager.h"

#include <QDebug>

JogControlManager::JogControlManager(OMNetwork* c_net, AxisOptions* c_opts, LiveDeviceModel* c_ldm, QComboBox* c_jogCombo, QDial* c_jogDial, QDoubleSpinBox* c_jogSpd, QDoubleSpinBox *c_jogDmp, QObject *parent) :
    QObject(parent)
{
    m_curAxis = 0;
    m_curRes = 1;

    m_jogCombo = c_jogCombo;
    m_jogSpd = c_jogSpd;
    m_jogDmp = c_jogDmp;
    m_jogDial = c_jogDial;
    m_net = c_net;
    m_opts = c_opts;
    m_ldm = c_ldm;

        // populate resolution combo

    m_jogCombo->addItem("Rapid", OM_JOGRES_RAPID);
    m_jogCombo->addItem("Coarse", OM_JOGRES_COARSE);
    m_jogCombo->addItem("Fine", OM_JOGRES_FINE);
    m_jogCombo->addItem("X Fine", OM_JOGRES_XFINE);
    m_jogCombo->setCurrentIndex(0);

        // create speedcontrolproxy object and run timer loop
        // in its own thread
    m_scp = new SpeedControlProxy(m_opts);
    m_scpThread = new QThread;

        // ensure the SCP knows when a new device has been added
    QObject::connect(m_net, SIGNAL(deviceAdded(OMdeviceInfo*)), m_scp, SLOT(deviceAdded(OMdeviceInfo*)));
    QObject::connect(m_net, SIGNAL(deviceDeleted(QString,unsigned short)), m_scp, SLOT(deviceRemoved(QString,unsigned short)));

        // inform SCP of a new device selected for jog control (via livedevicemodel)
    QObject::connect(m_ldm, SIGNAL(deviceSelected(unsigned short)), m_scp, SLOT(deviceChange(unsigned short)), Qt::QueuedConnection);

        // of course, SCP may not allow the change yet (still moving to target speed)
        // so we'll need to pass this signal up as needed

    QObject::connect(m_scp, SIGNAL(motorNotReady(unsigned short)), this, SIGNAL(motorChangeDenied(unsigned short)), Qt::QueuedConnection);

        // we need to know when a device is changed so that we can modify the jogspeed and jogdamp spin boxes as needed
    QObject::connect(m_scp, SIGNAL(motorChangeAccepted(unsigned short)), this, SLOT(_liveDeviceSelected(unsigned short)), Qt::QueuedConnection);


        // listen to jog spinners
    QObject::connect(m_jogSpd, SIGNAL(valueChanged(double)), this, SLOT(_jogMaxSpeedChange(double)), Qt::QueuedConnection);
    QObject::connect(m_jogDmp, SIGNAL(valueChanged(double)), this, SLOT(_jogDampChange(double)), Qt::QueuedConnection);

        // tie the jog dial into the speed control proxy
    QObject::connect(m_jogDial, SIGNAL(sliderMoved(int)), m_scp, SLOT(speedPosChange(int)), Qt::QueuedConnection);

        // tie resolution change to us (we'll pass onto SCP)
    QObject::connect(m_jogCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(_jogResChange(int)), Qt::QueuedConnection);




        // move speedcontrolproxy to thread and start it
    m_scp->moveToThread(m_scpThread);
    m_scpThread->start();
        // do this after starting the thread to ensure the timer
        // has affinity to the new thread, rather than ours
    m_scp->startDampTimer();


}

JogControlManager::~JogControlManager() {

        // stop the timer and thread for SCP
    m_scp->stopDampTimer();
    m_scpThread->quit();
    m_scpThread->wait();


    delete m_scpThread;
    delete m_scp;
}

void JogControlManager::_liveDeviceSelected(unsigned short p_addr) {

    qDebug() << "JCM: Received selection for device addr" << p_addr;
    _prepJogInputs(p_addr);
        // revert to rapid
    m_jogCombo->setCurrentIndex(0);


}

double JogControlManager::_stepsToJogSpeed(OMaxisOptions* p_opts, unsigned int p_steps) {
    float setMove = 360.0;

    if( p_opts->axisType != AXIS_MOVE_ROT )
        setMove = 1.0;

    double spd = ( ((setMove / p_opts->ratio ) * (float) p_steps) * 60.0 ) / m_curRes;



    return( spd );

}

unsigned int JogControlManager::_jogSpeedToSteps(OMaxisOptions *p_opts, double p_speed) {
    float setMove = 360.0;

    if( p_opts->axisType != AXIS_MOVE_ROT )
        setMove = 1.0;

    return( ((p_speed / 60.0) / (setMove / p_opts->ratio)) * m_curRes );

}

void JogControlManager::_prepJogInputs(unsigned short p_addr) {

        // update the jog speed limit and damping with the saved
        // values for the axis
    OMaxisOptions* opts = m_opts->getOptions(p_addr);

    unsigned int jog_limit = opts->jogLimit;
    unsigned int max_jog = opts->maxSteps;
    // float ratio = opts->ratio;
    unsigned int type = opts->axisType;
    float setMove = 360.0;

    if( type != AXIS_MOVE_ROT )
        setMove = 1.0;


    float dispMax = _stepsToJogSpeed(opts, max_jog);
    float curMax  = _stepsToJogSpeed(opts, jog_limit);

    qDebug() << "JCM: Setting current speed max value to" << curMax;

    m_jogSpd->setMaximum(dispMax);
    m_jogSpd->setValue(curMax);


    m_jogDmp->setMaximum(30);
    m_jogDmp->setValue(opts->jogDamp);

    m_curAxis = p_addr;

}

void JogControlManager::_jogMaxSpeedChange(double p_spd) {
    if( m_curAxis == 0 )
        return;

    qDebug() << "JCM: Got max jog speed limit change to " << p_spd;

    OMaxisOptions* opts = m_opts->getOptions(m_curAxis);

    unsigned int steps = _jogSpeedToSteps(opts, p_spd);
    double spd_pct = (double) steps / (double) opts->maxSteps;

    qDebug() << "JCM: Steps =" << steps << "Percentage:" << spd_pct;

    m_scp->maxSpeed(spd_pct);

        // record and save new value
    opts->jogLimit = steps;
    m_opts->setOptions(m_curAxis, opts);

}

void JogControlManager::_jogDampChange(double p_damp) {
    if( m_curAxis == 0 )
        return;

    qDebug() << "FW: Got jog damp change to " << p_damp;

    m_scp->setDamping(p_damp);

        // save damping value for axis
    OMaxisOptions* opts = m_opts->getOptions(m_curAxis);
    opts->jogDamp = p_damp;
    m_opts->setOptions(m_curAxis, opts);
}

// Handle changes to resolution combobox
// we do this here so that the SCP doesn't need to know
// anything about the combobox its self

void JogControlManager::_jogResChange(int p_idx) {
    unsigned int ms = m_jogCombo->itemData(p_idx).toUInt();

    m_scp->setResolution(ms);

    OMaxisOptions* opts = m_opts->getOptions(m_curAxis);

    m_curRes = ms;

        // update max jog speed display

    double newDisp = _stepsToJogSpeed(opts, opts->jogLimit);
    double dispMax = _stepsToJogSpeed(opts, opts->maxSteps);

    m_jogSpd->setMaximum(dispMax);
    m_jogSpd->setValue(newDisp);



}
