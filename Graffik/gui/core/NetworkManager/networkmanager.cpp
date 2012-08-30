#include <QDebug>

#include "networkmanager.h"
#include "ui_networkmanager.h"

#include "core/Options/axisoptionsdialog.h"


NetworkManager::NetworkManager(NetworkModel *c_net, AxisOptions* c_opts, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NetworkManager)
{
    ui->setupUi(this);

    _netMod = c_net;
    _opts = c_opts;

    ui->netTreeView->setModel(c_net);

    ui->netTreeView->expandAll();

    QObject::connect(ui->netTreeView, SIGNAL(clicked(QModelIndex)), this, SLOT(onRowClick(QModelIndex)), Qt::QueuedConnection);

}

NetworkManager::~NetworkManager()
{
    delete ui;
}

void NetworkManager::on_addBusButton_clicked() {
    AddNetDialog addNet(_netMod->net(), this);
    addNet.exec();
}

void NetworkManager::on_addDevButton_clicked() {
    AddDeviceDialog addDev(_netMod->net(), this);
    addDev.exec();
}

void NetworkManager::on_delDevButton_clicked() {
    DeleteDeviceDialog delDev(_netMod->net(), this);
    delDev.exec();
}

void NetworkManager::on_delBusButton_clicked() {
    DeleteBusDialog delBus(_netMod->net(), this);
    delBus.exec();
}

void NetworkManager::onRowClick(QModelIndex p_idx) {
    qDebug() << "NM: Got click on" << p_idx.row() << p_idx.column();

        // if they clicked the config button, retrieve address for item
    if( p_idx.column() == NETMODEL_CONFIG_COL ) {
        qDebug() << "NM: Hit config link";

            // model is nested by bus...
        const QModelIndex parent = p_idx.parent();
        QModelIndex nidx = _netMod->index(p_idx.row(), NETMODEL_ADDR_COL, parent);
            // get item
        const QStandardItem* itm = _netMod->itemFromIndex(nidx);

        unsigned short addr = itm->text().toUShort();
        qDebug() << "NM: Spawn Config for address" << addr;

        AxisOptionsDialog opts(_opts, addr, this);
        opts.exec();
    }
}

