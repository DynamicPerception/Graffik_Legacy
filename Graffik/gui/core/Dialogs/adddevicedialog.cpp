#include <QList>
#include <QVariant>
#include <QIntValidator>
#include <QDebug>


#include "adddevicedialog.h"
#include "ui_adddevicedialog.h"

#include "core/Dialogs/errordialog.h"

AddDeviceDialog::AddDeviceDialog(OMNetwork *c_net, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddDeviceDialog)
{
    ui->setupUi(this);
    _net = c_net;

    _initInputs();
}


AddDeviceDialog::~AddDeviceDialog()
{
    delete ui;
 }


void AddDeviceDialog::_initInputs() {
    QStringList validDevices = _net->deviceTypes();

    foreach( QString type, validDevices ) {
        ui->deviceTypeList->addItem(type);
    }

    ui->devAddr->setValidator(new QIntValidator(this));

    updateBuses();

}


void AddDeviceDialog::updateBuses() {
    QList<QString> usedPorts = _net->getBuses();

        // clear out the combo box
    while( ui->busList->count() > 0 ) {
        ui->busList->removeItem( ui->busList->count() - 1 );
    }

    foreach( QString port, usedPorts ) {
        qDebug() << "Found port " << port;
        OMbusInfo* bus = _net->busInfo(port);
        ui->busList->addItem(bus->name, QVariant(port));
    }

}

void AddDeviceDialog::accept() {
    QString port = ui->busList->itemData(ui->busList->currentIndex()).toString();
    QString type = ui->deviceTypeList->itemText(ui->deviceTypeList->currentIndex());
    QString name = ui->devName->text();
    QString addr = ui->devAddr->text();

    unsigned short d_addr = addr.toUShort();

    bool ok = true;
    QString errString = "";

    if( name.isEmpty() ) {
        errString = "You must enter a name for this device.";
        ok = false;
    }

    if( addr.isEmpty() ) {
        errString = "You must enter an address for this device.";
        ok = false;
    }

    if( ok == true ) {
        try {
            _net->addDevice(port, d_addr, type, name);
        }
        catch (int e) {
            ok = false;
            if( e == OM_NET_DUPE ) {
                errString = "The address is already in-use";
            }
            else {
                errString = "An unknown error occured, please try again.";
            }
        }
    }


    if( ok == false ) {
        // throw out error dialog
        ErrorDialog er(this);
        er.setError(errString);
        er.exec();
    }
    else {
        this->done(1);
    }

}
