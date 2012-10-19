#include <QList>
#include <QVariant>
#include <QIntValidator>
#include <QDebug>


#include "adddevicedialog.h"
#include "ui_adddevicedialog.h"

#include "core/Dialogs/errordialog.h"

/** Constructor for adding an unknown device

  Displays a dialog with all known buses, types, etc. For the user to input data
  about a new device to be added.

  @param c_net
  A pointer to a valid OMNetwork object
  */

AddDeviceDialog::AddDeviceDialog(OMNetwork *c_net, QWidget *parent) : QDialog(parent), ui(new Ui::AddDeviceDialog) {
    ui->setupUi(this);
    _net = c_net;

    _initInputs();

    QWidget::setTabOrder(ui->busList, ui->deviceTypeList);
    QWidget::setTabOrder(ui->deviceTypeList, ui->devAddr);
    QWidget::setTabOrder(ui->devAddr, ui->devName);

}

/** Constructor for adding a specific, known, device

    Displays a dialog with the bus, device type, and address pre-selected.  The user may not
    change these values.

    @param c_net
    A pointer to a valid OMNetwork object

    @param c_bus
    A valid bus port

    @param c_type
    A valid bus type, or an identifer that can be used to find a valid bus type.  It can either be a Device Type Name or
    a Device ID as returned from OMDevice::getId()

    @param c_addr
    The address of the new device

    @param c_name
    The name of the new device

    @param parent
    The parent widget

    If the type does not exist, or is unknown, an error dialog will be presented instead

  */

AddDeviceDialog::AddDeviceDialog(OMNetwork *c_net, QString c_bus, QString c_type, unsigned short c_addr, QString c_name, QWidget *parent) : QDialog(parent), ui(new Ui::AddDeviceDialog) {

    ui->setupUi(this);
    _net = c_net;
    _initInputs();

    QString busName = _net->busInfo(c_bus)->name;

    if( ! _net->deviceTypes().contains(c_type) )
        c_type = _net->deviceIDtoType(c_type);

    if( c_type.length() < 1 ) {
        // throw out error dialog
        ErrorDialog er(this);
        er.setError("Cannot Add Device:\n\tUnknown Type: " + c_type);
        er.exec();
        done(1);
        return;
    }

    int busLoc = ui->busList->findText(busName);
    ui->busList->setCurrentIndex(busLoc);
    ui->busList->setEnabled(false);

    int devLoc = ui->deviceTypeList->findText(c_type);
    ui->deviceTypeList->setCurrentIndex(devLoc);
    ui->deviceTypeList->setEnabled(false);

    ui->devAddr->setText(QString::number(c_addr));
    ui->devAddr->setEnabled(false);

    ui->devName->setText(c_name);
    ui->devAddr->setEnabled(false);

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
        qDebug() << "ADDev: Found port " << port;
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
