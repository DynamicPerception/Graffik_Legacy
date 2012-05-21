#include "networkmanager.h"
#include "ui_networkmanager.h"

networkManager::networkManager(networkModel *c_net, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::networkManager)
{
    ui->setupUi(this);

    _netMod = c_net;

    ui->netTreeView->setModel(c_net);

    ui->netTreeView->expandAll();

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

