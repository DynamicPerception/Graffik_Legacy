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
