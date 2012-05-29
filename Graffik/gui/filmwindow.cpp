#include "filmwindow.h"
#include "ui_filmwindow.h"

#include <QDebug>

filmWindow::filmWindow(OMNetwork* c_net, OMAxisFilmOptions *c_opts, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::filmWindow)
{
    ui->setupUi(this);
    _net = c_net;
    _ldModel = new LiveDeviceModel(this);
    _opts = c_opts;
    _scp = new SpeedControlProxy(_opts);
    _scpThread = new QThread;

    m_curAxis = 0;

    ui->devButtonList->setModel(_ldModel);

        // allow these film-specific objects to know when a new device has been added

    QObject::connect(_net, SIGNAL(deviceAdded(OMdeviceInfo*)), _ldModel, SLOT(deviceAdded(OMdeviceInfo*)), Qt::QueuedConnection);
    QObject::connect(_net, SIGNAL(deviceAdded(OMdeviceInfo*)), _scp, SLOT(deviceAdded(OMdeviceInfo*)), Qt::QueuedConnection);

        // pass a click on to the model via signal
    QObject::connect(ui->devButtonList, SIGNAL(clicked(const QModelIndex &)), _ldModel, SLOT(deviceClicked(const QModelIndex &)), Qt::QueuedConnection);

        // inform all needed UI elements of a new device selected for jog control (via livedevicemodel)
    QObject::connect(_ldModel, SIGNAL(deviceSelected(unsigned short)), _scp, SLOT(deviceChange(unsigned short)), Qt::QueuedConnection);

        // we also listen in, so that we can change a few UI elements not worthy of their own classes
    QObject::connect(_ldModel, SIGNAL(deviceSelected(unsigned short)), this, SLOT(_liveDeviceSelected(unsigned short)), Qt::QueuedConnection);

        // tie the jog dial into the speed control proxy
    QObject::connect(ui->jogDial, SIGNAL(sliderMoved(int)), _scp, SLOT(speedPosChange(int)), Qt::QueuedConnection);

        // listen to jog spinners
    QObject::connect(ui->jogSpeedSpin, SIGNAL(valueChanged(double)), this, SLOT(_jogMaxSpeedChange(double)), Qt::QueuedConnection);
    QObject::connect(ui->jogDampSpin, SIGNAL(valueChanged(double)), this, SLOT(_jogDampChange(double)), Qt::QueuedConnection);

        // move speedcontrolproxy to thread and start it
    _scp->moveToThread(_scpThread);
    _scpThread->start();

    _scp->startDampTimer();

    m_threadStarted = true;

//    ui->devButtonList->setMovement(QListView::Free);
//    ui->devButtonList->setDragDropMode(QAbstractItemView::InternalMove);
}

filmWindow::~filmWindow()
{
    delete ui;
        // make sure to stop this thread first!
   // if( m_threadStarted ) {
        _scp->stopDampTimer();
        _scpThread->quit();
        _scpThread->wait();
   // }

    delete _scpThread;
    delete _scp;
    delete _ldModel;
}

void filmWindow::_liveDeviceSelected(unsigned short p_addr) {

    qDebug() << "FW: Received selection for device addr" << p_addr;
    _prepJogInputs(p_addr);


}

double filmWindow::_stepsToJogSpeed(OMaxisOptions* p_opts, unsigned int p_steps) {
    float setMove = 360.0;

    if( p_opts->axisType != AXIS_MOVE_ROT )
        setMove = 1.0;

    return( ((setMove / p_opts->ratio ) * (float) p_steps) * 60.0 );

}

unsigned int filmWindow::_jogSpeedToSteps(OMaxisOptions *p_opts, double p_speed) {
    float setMove = 360.0;

    if( p_opts->axisType != AXIS_MOVE_ROT )
        setMove = 1.0;

    return( (p_speed / 60.0) / (setMove / p_opts->ratio ) );

}

void filmWindow::_prepJogInputs(unsigned short p_addr) {

        // update the jog speed limit and damping with the saved
        // values for the axis
    OMaxisOptions* opts = _opts->getOptions(p_addr);

    unsigned int jog_limit = opts->jogLimit;
    unsigned int max_jog = opts->maxSteps;
    float ratio = opts->ratio;
    unsigned int type = opts->axisType;
    float setMove = 360.0;

    if( type != AXIS_MOVE_ROT )
        setMove = 1.0;


    float dispMax = _stepsToJogSpeed(opts, max_jog);
    float curMax  = _stepsToJogSpeed(opts, jog_limit);

    qDebug() << "FW: Setting current speed max value to" << curMax;

    ui->jogSpeedSpin->setMaximum(dispMax);
    ui->jogSpeedSpin->setValue(curMax);


    ui->jogDampSpin->setMaximum(30);
    ui->jogDampSpin->setValue(opts->jogDamp);

    m_curAxis = p_addr;

}

void filmWindow::_jogMaxSpeedChange(double p_spd) {
    if( m_curAxis == 0 )
        return;

    qDebug() << "FW: Got max jog speed limit change to " << p_spd;

    OMaxisOptions* opts = _opts->getOptions(m_curAxis);

    unsigned int steps = _jogSpeedToSteps(opts, p_spd);
    double spd_pct = (double) steps / (double) opts->maxSteps;

    qDebug() << "FW: Steps =" << steps << "Percentage:" << spd_pct;

    _scp->maxSpeed(spd_pct);

        // record and save new value
    opts->jogLimit = steps;
    _opts->setOptions(m_curAxis, opts);

}

void filmWindow::_jogDampChange(double p_damp) {
    if( m_curAxis == 0 )
        return;

    qDebug() << "FW: Got jog damp change to " << p_damp;

    _scp->setDamping(p_damp);

        // save damping value for axis
    OMaxisOptions* opts = _opts->getOptions(m_curAxis);
    opts->jogDamp = p_damp;
    _opts->setOptions(m_curAxis, opts);
}
