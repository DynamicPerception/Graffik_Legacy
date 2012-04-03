#include "filmwindow.h"
#include "ui_filmwindow.h"

filmWindow::filmWindow(OMNetwork* c_net, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::filmWindow)
{
    ui->setupUi(this);
    _net = c_net;
    _ldModel = new LiveDeviceModel(_net, this);
    _scp = new SpeedControlProxy(_net, _ldModel, this);

    ui->devButtonList->setModel(_ldModel);
        // pass a click on to the model via signal
    QObject::connect(ui->devButtonList, SIGNAL(clicked(const QModelIndex &))), _ldModel, SLOT(deviceClicked(const QModelIndex &), Qt::QueuedConnection);

//    ui->devButtonList->setMovement(QListView::Free);
//    ui->devButtonList->setDragDropMode(QAbstractItemView::InternalMove);
}

filmWindow::~filmWindow()
{
    delete ui;
    delete _scp;
    delete _ldModel;
}
