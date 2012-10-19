#include "deviceassigndialog.h"
#include "ui_deviceassigndialog.h"

#include "core/Dialogs/errordialog.h"

#include <QDebug>

DeviceAssignDialog::DeviceAssignDialog(OMNetwork *c_net, QString c_bus, QString c_type, QString c_name, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeviceAssignDialog)
{
    ui->setupUi(this);

    m_net = c_net;
    m_bus = c_bus;
    m_type = c_type;
    m_name = c_name;
    m_cmdId = 0;

    _initInputs(m_type);

    QObject::connect(m_net, SIGNAL(complete(int,OMCommandBuffer*)), this, SLOT(_commandComplete(int,OMCommandBuffer*)), Qt::QueuedConnection);
    QObject::connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

DeviceAssignDialog::~DeviceAssignDialog()
{
    delete ui;
}

void DeviceAssignDialog::_initInputs(QString p_type) {
    if( ! m_net->deviceTypes().contains(m_type) )
        m_type = m_net->deviceIDtoType(m_type);

    if( m_type.length() < 1 ) {
        ErrorDialog er;
        er.setError("I'm sorry, but I don't know anything about the device type " + p_type + "\nCowardly refusing to attempt to add it.");
        er.exec();
        done(1);
        return;
    }

    QHash<unsigned short, OMdeviceInfo*> existDevs = m_net->getDevices();

        // populate a list of valid, unused addresses
    for(int i = 3; i <= 255; i++) {
        if( ! existDevs.contains(i) ) {
            ui->addrCombo->addItem(QString::number(i));
        }
    }

    ui->nameLine->setText(m_name);

}

void DeviceAssignDialog::accept() {

    qDebug() << "DAD: Got accept signal";

    m_newAddr = ui->addrCombo->currentText().toUShort();
    m_newName = ui->nameLine->text();

    if( m_newName.length() < 1 ) {
        ErrorDialog er;
        er.setError("You must provide a name for this device");
        er.exec();
        return;
    }

    qDebug() << "DAD: Got new address and name: " << m_newAddr << m_newName;

        // we need to add the device as address 2 first
        // we turn off broadcast, as we don't want anyone to see
        // this device
    m_net->addDevice(m_bus, 2, m_type, m_newName, false);

        // send an address change command

    OMdeviceInfo* dev = m_net->deviceInfo(m_bus, 2);
    m_cmdId = dev->device->changeAddress(m_newAddr);
    m_net->getManager()->hold(m_cmdId);

        // we don't accept or act like we're done here.  We're going to wait
        // for our command to return first!

}

void DeviceAssignDialog::_commandComplete(int p_id, OMCommandBuffer *p_cmd) {
    if( p_id != m_cmdId )
        return;

        // we don't need this device any more!

    m_net->deleteDevice(m_bus, 2, false);

    if( p_cmd->status() != OMC_SUCCESS ) {
            // well, that's just painful! How dare someone not follow the spec!
        ErrorDialog er;
        er.setError("The device did not respond positively to a required command");
        er.exec();
        m_net->getManager()->release(m_cmdId);
        done(1);
    }

    // now, we add the really real device.

    m_net->addDevice(m_bus, m_newAddr, m_type, m_newName, true);

    _postInit(m_bus, m_newAddr, m_type, m_newName);

    done(1);
}

void DeviceAssignDialog::_postInit(QString p_bus, unsigned short p_addr, QString p_type, QString p_name) {

    OMdeviceInfo* dev = m_net->deviceInfo(p_bus, p_addr);
    qDebug() << "DAD: Post-init for device " << p_bus << p_addr << p_type << p_name;

        // For OMAXISVX types, we send a (blind) name change command
    if( p_type == "OpenMoCo Axis") {
        qDebug() << "DAD: Setting name for device " << p_bus << p_addr << p_type << p_name;
        OMAxis* omaxis = dynamic_cast<OMAxis*>(dev->device);
        omaxis->name(p_name);
    }
}
