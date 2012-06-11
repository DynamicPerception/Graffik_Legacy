#include <QDebug>

#include "networkmanager.h"
#include "ui_networkmanager.h"

#include "film/OMAxisFilmOptions/omaxisfilmoptionsdialog.h"


networkManager::networkManager(networkModel *c_net, OMAxisFilmOptions* c_opts, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::networkManager)
{
    ui->setupUi(this);

    _netMod = c_net;
    _opts = c_opts;

    ui->netTreeView->setModel(c_net);

    ui->netTreeView->expandAll();

    QObject::connect(ui->netTreeView, SIGNAL(clicked(QModelIndex)), this, SLOT(onRowClick(QModelIndex)), Qt::QueuedConnection);

}

networkManager::~networkManager()
{
    delete ui;
}

void networkManager::on_addBusButton_clicked() {
    addNetDialog addNet(_netMod->net(), this);
    addNet.exec();
}

void networkManager::on_addDevButton_clicked() {
    addDeviceDialog addDev(_netMod->net(), this);
    addDev.exec();
}

void networkManager::on_delDevButton_clicked() {
    DeleteDeviceDialog delDev(_netMod->net(), this);
    delDev.exec();
}

void networkManager::onRowClick(QModelIndex p_idx) {
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

        OMAxisFilmOptionsDialog opts(_opts, addr, this);
        opts.exec();
    }
}

