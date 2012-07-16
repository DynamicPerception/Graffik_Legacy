#include "filmwindow.h"
#include "ui_filmwindow.h"

#include <QDebug>


FilmWindow::FilmWindow(OMNetwork* c_net, AxisOptions *c_opts, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FilmWindow)
{
    ui->setupUi(this);

    m_net = c_net;
    m_opts = c_opts;

    m_ldModel = new LiveDeviceModel(m_net, this);
    m_jcm = new JogControlManager(m_net, m_opts, m_ldModel, ui->jogResCombo, ui->jogDial, ui->jogSpeedSpin, ui->jogDampSpin, this);
    m_params = new FilmParameters(m_net, this);


        // connect the device list display to the live device model
    ui->devButtonList->setModel(m_ldModel);


    m_areaLayout = new QVBoxLayout;
    ui->visualSAContents->setLayout(m_areaLayout);

        // we need to populate motion area displays
    QObject::connect(m_net, SIGNAL(deviceAdded(OMdeviceInfo*)), this, SLOT(_drawNewAxis(OMdeviceInfo*)));
    QObject::connect(m_net, SIGNAL(deviceDeleted(QString,unsigned short)), this, SLOT(_eraseAxis(QString,unsigned short)));

        // pass a click on to the model via signal
    QObject::connect(ui->devButtonList, SIGNAL(clicked(const QModelIndex &)), m_ldModel, SLOT(deviceClicked(const QModelIndex &)));

    QObject::connect(m_jcm, SIGNAL(motorChangeDenied(unsigned short)), this, SLOT(_jogMotorChangeDenied(unsigned short)));


    _prepInputs();

//    ui->devButtonList->setMovement(QListView::Free);
//    ui->devButtonList->setDragDropMode(QAbstractItemView::InternalMove);
}

FilmWindow::~FilmWindow()
{
    foreach(unsigned short addr, m_areaBlocks.keys()) {
        delete m_areaBlocks.value(addr);
        m_areaBlocks.remove(addr);
    }

    delete m_areaLayout;

    delete m_params;
    delete m_jcm;
    delete m_ldModel;
    delete ui;
}

void FilmWindow::_jogMotorChangeDenied(unsigned short p_oldAddr) {
    qDebug() << "FW: Motor change was denied, re-setting devButtonList selection" << p_oldAddr;

    int row = m_ldModel->find(p_oldAddr);

    qDebug() << "FW: New Row =" << row;

    QModelIndex tl = m_ldModel->index(row, 0, QModelIndex());
    QModelIndex tr = m_ldModel->index(row, 0, QModelIndex());
    QItemSelection sel(tl, tr);
    ui->devButtonList->selectionModel()->select(sel, QItemSelectionModel::SelectCurrent);
}

void FilmWindow::_drawNewAxis(OMdeviceInfo *p_dev) {

        // don't show a motion area for device types that
        // do not support it

    if( p_dev->type != "OpenMoCo Axis")
        return;

    qDebug() << "FW: Got new axis, spawning motion area" << p_dev->type;

    MotionBase* area = new MotionBase(p_dev, this);
    m_areaBlocks.insert(p_dev->device->address(), area);
    m_areaLayout->addWidget(area);
}

void FilmWindow::_eraseAxis(QString p_bus, unsigned short p_addr) {
    qDebug() << "FW: Got erase axis" << p_bus << p_addr;
    if( m_areaBlocks.contains(p_addr) ) {
        m_areaLayout->removeWidget(m_areaBlocks.value(p_addr));
        delete m_areaBlocks.value(p_addr);
        m_areaBlocks.remove(p_addr);
    }
}

void FilmWindow::on_camControlCheckBox_stateChanged(int p_state) {

    bool camControl = false;

    if( p_state == Qt::Checked )
        camControl = true;

    qDebug() << "FW: Cam Control Set to: " << camControl;

    _enableCamControl(camControl);

}

void FilmWindow::_enableCamControl(bool p_en) {

    OMfilmParams* params = m_params->getParams();
    params->camParams->camControl = p_en;
    bool manInt = params->camParams->manInterval;
    m_params->releaseParams();

    ui->camSetBut->setEnabled(p_en);

        // do not enable "Film time" controls if manual interval is
        // enabled

    if( manInt )
        p_en = false;

    ui->filmHHSpin->setEnabled(p_en);
    ui->filmMMSpin->setEnabled(p_en);
    ui->filmSSSpin->setEnabled(p_en);



}

void FilmWindow::on_camSetBut_clicked() {
    CameraControlDialog* control = new CameraControlDialog(m_params);
    QObject::connect(control, SIGNAL(intervalControlChanged(bool)), this, SLOT(_manIntervalChange(bool)));
    control->exec();
    delete control;
}

void FilmWindow::_manIntervalChange(bool p_en) {
   /* ui->filmHHSpin->setEnabled(!p_en);
    ui->filmMMSpin->setEnabled(!p_en);
    ui->filmSSSpin->setEnabled(!p_en);*/
}

void FilmWindow::_showFilmTime() {
    OMfilmParams* params = m_params->getParams();
    unsigned long wallTM = params->realLength;
    unsigned long filmTM = params->length;
    m_params->releaseParams();

    unsigned long whh = wallTM / 1000 / 60 / 60;
    unsigned long wmm = (wallTM - (whh * 1000 * 60 * 60)) / 1000 / 60;
    unsigned long wss = (wallTM - (whh * 1000 * 60 * 60) - (wmm * 1000 * 60)) / 1000;

    unsigned long fhh = filmTM / 1000 / 60 / 60;
    unsigned long fmm = (filmTM - (fhh * 1000 * 60 * 60)) / 1000 / 60;
    unsigned long fss = (filmTM - (fhh * 1000 * 60 * 60) - (fmm * 1000 * 60)) / 1000;

    ui->filmHHSpin->setValue(fhh);
    ui->filmMMSpin->setValue(fmm);
    ui->filmSSSpin->setValue(fss);

    ui->realHHSpin->setValue(whh);
    ui->realMMSpin->setValue(wmm);
    ui->realSSSpin->setValue(wss);

}

void FilmWindow::_prepInputs() {
        // default is no camera control
    _enableCamControl(false);

    _showFilmTime();

    ui->filmHHSpin->setMaximum(999);
    ui->filmMMSpin->setMaximum(59);
    ui->filmSSSpin->setMaximum(59);

    ui->realHHSpin->setMaximum(999);
    ui->realMMSpin->setMaximum(59);
    ui->realSSSpin->setMaximum(59);

}

 // convert inputs to mS time and write back to film parameters
void FilmWindow::_changeTime(int p_which, int p_pos, int p_val) {

    int hh; int mm; int ss;

    if( p_which == 1 ) {
        hh = ui->filmHHSpin->value();
        mm = ui->filmMMSpin->value();
        ss = ui->filmSSSpin->value();
    }
    else {
        hh = ui->realHHSpin->value();
        mm = ui->realMMSpin->value();
        ss = ui->realSSSpin->value();
    }

    if( p_pos == 1 )
        hh = p_val;
    else if( p_pos == 2 )
        mm = p_val;
    else
        ss = p_val;

    unsigned long mS = (hh * 60 * 60 * 1000) + (mm * 60 * 1000) + (ss * 1000);

    OMfilmParams* params = m_params->getParams();

    if( p_which == 1 )
        params->length = mS;
    else
        params->realLength = mS;

    m_params->releaseParams();

}

 // apply constraints to film length, and update display as needed
void FilmWindow::_checkFilmTimeConstraint() {
    OMfilmParams* params = m_params->getParams();
    bool do_adjust = false;

    if( params->length > params->realLength ) {
        params->length = params->realLength;
        do_adjust = true;
    }

    m_params->releaseParams();

    if( do_adjust ) {
        _showFilmTime();
        qDebug() << "FW: Constraining maximum film time to real time";
    }
}

void FilmWindow::on_filmHHSpin_valueChanged(int p_val) {
    _changeTime(1, 1, p_val);
    _checkFilmTimeConstraint();
}

void FilmWindow::on_filmMMSpin_valueChanged(int p_val) {
    _changeTime(1, 2, p_val);
    _checkFilmTimeConstraint();}

void FilmWindow::on_filmSSSpin_valueChanged(int p_val) {
    _changeTime(1, 3, p_val);
    _checkFilmTimeConstraint();
}

void FilmWindow::on_realHHSpin_valueChanged(int p_val) {
    _changeTime(2, 1, p_val);
}

void FilmWindow::on_realMMSpin_valueChanged(int p_val) {
    _changeTime(2, 2, p_val);
}

void FilmWindow::on_realSSSpin_valueChanged(int p_val) {
    _changeTime(2, 3, p_val);
}
