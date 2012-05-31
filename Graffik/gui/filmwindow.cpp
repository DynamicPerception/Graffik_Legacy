#include "filmwindow.h"
#include "ui_filmwindow.h"

#include <QDebug>

filmWindow::filmWindow(OMNetwork* c_net, OMAxisFilmOptions *c_opts, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::filmWindow)
{
    ui->setupUi(this);

    m_net = c_net;
    m_opts = c_opts;

    m_ldModel = new LiveDeviceModel(this);
    m_jcm = new JogControlManager(m_net, m_opts, m_ldModel, ui->jogResCombo, ui->jogDial, ui->jogSpeedSpin, ui->jogDampSpin, this);



        // connect the device list display to the live device model
    ui->devButtonList->setModel(m_ldModel);

        // allow these film-specific objects to know when a new device has been added

    QObject::connect(m_net, SIGNAL(deviceAdded(OMdeviceInfo*)), m_ldModel, SLOT(deviceAdded(OMdeviceInfo*)), Qt::QueuedConnection);

        // pass a click on to the model via signal
    QObject::connect(ui->devButtonList, SIGNAL(clicked(const QModelIndex &)), m_ldModel, SLOT(deviceClicked(const QModelIndex &)), Qt::QueuedConnection);

//    ui->devButtonList->setMovement(QListView::Free);
//    ui->devButtonList->setDragDropMode(QAbstractItemView::InternalMove);
}

filmWindow::~filmWindow()
{
    delete m_jcm;
    delete m_ldModel;
    delete ui;
}

