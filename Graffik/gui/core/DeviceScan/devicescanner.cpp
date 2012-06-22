#include "devicescanner.h"

#include <QDebug>

#include "core/Dialogs/errordialog.h"
#include "core/Dialogs/adddevicedialog.h"

/** Create an instance of the class

  The DeviceScanner displays a dialog while scanning the network for devices.  Once devices are found, it will
  present the Add Device dialog for each device to configure it, with certain fields pre-populated.

  Note that the scanner will -not- scan any address that has already been added as a device, and will only scan if
  a bus has been added to the network.

  */

DeviceScanner::DeviceScanner(OMNetwork *c_net, QObject *parent) : QObject(parent) {

    _initScanner(c_net, 0);
    _scan(m_findAddr);
}

/**
  If an address is provided as part of the constructor arguments, it will ponly scan for a device at that
  address across all buses.  It will stop after it finds the first device.  In this case, instead of the normal
  add device dialog being presented, a special dialog will be presented to provide a new address and a name for
  the device.  This is primarily intended for use when initializing a new device (i.e. address '2'), and needing
  to set it to a new address.  After the dialog has been presented, the device will be added with the user-supplied
  information.
*/

DeviceScanner::DeviceScanner(OMNetwork *c_net, unsigned short c_addr, QObject *parent) : QObject(parent) {

    _initScanner(c_net, c_addr);

        // we don't do squat if the address is already in-use
    if( _validateAddress(c_addr) )
        _scan(m_findAddr);
}


DeviceScanner::~DeviceScanner() {
    delete m_scanDialog;
}

bool DeviceScanner::_validateAddress(unsigned short p_addr) {
    if( p_addr != 0 && m_net->getDevices().contains(p_addr) ) {
        ErrorDialog er;
        er.setError("Cannot initialize device at address " + QString::number(p_addr) + " because a device is already on the network with that address. Remove that device before attempting to initialize it.");
        er.exec();
        return false;
    }

    return true;
}

void DeviceScanner::_initScanner(OMNetwork *c_net, unsigned short p_addr) {
    m_net = c_net;
    m_cmd = m_net->getManager();
    m_scanDialog = new DeviceScanDialog();
    m_scanDialog->setModal(true);
    m_findAddr = p_addr;

        // we need to listen for our commands to see if something is alive out there..
    QObject::connect(m_net, SIGNAL(complete(int, OMCommandBuffer*)), this, SLOT(_commandCompleted(int, OMCommandBuffer*)), Qt::QueuedConnection);

}

/** Scans for devices at an address, or use address 0 for all addresses not currently assigned
  */

void DeviceScanner::_scan(unsigned short p_addr) {

    qDebug() << "DS: Scanning for Devices";

    m_foundCount = 0;
    m_respCount = 0;

    QList<QString> buses = m_net->getBuses();

    if( buses.length() < 1 ) {
        ErrorDialog er;
        er.setError("No Buses were found, please add at least one bus before scanning");
        er.exec();
        return;
    }

    QHash<unsigned short, OMdeviceInfo*> allKnownDevs = m_net->getDevices();

    m_scanDialog->enableConfirm(false);

        // Listen for the done button being clicked
    QObject::connect(m_scanDialog, SIGNAL(accepted()), this, SLOT(_scanAccepted()));

    if( p_addr != 0) {
        if( allKnownDevs.contains(p_addr) ) {
            ErrorDialog er;
            er.setError("A device already exists at address " + QString::number(p_addr) + ", Please remove that device before scanning that address");
            er.exec();
            return;
        }

        m_scanDialog->show();

        foreach( QString bus, buses ) {
            _sendRequest(bus, p_addr);
        }

        return;
    }

    m_scanDialog->show();

    for(int i = 2; i < 256; i++) {
        if( ! allKnownDevs.contains(i) ) {
            foreach(QString bus, buses)
                _sendRequest(bus, i);
        }

    }
}

 /** Sends a getId() bus command to specified address

   */

void DeviceScanner::_sendRequest(QString p_bus, unsigned short p_addr) {
    QString busPort = m_net->busInfo(p_bus)->bus->port();


    // we'll need to add a device, temporarily, to send a command to it
    try {
            // we add the device as an OM Axis, just so we can associate a known
            // type, we use a global protocol command, so it doesn't matter what's
            // on the other end.  We disable broadcasting to prevent the other UI
            // elements from picking these devices up, and we'll delete them when
            // there's some result from the command we send...

        m_net->addDevice(busPort, p_addr, "OpenMoCo Axis", "scanning", false);
    }
    catch(int e) {
        qDebug() << "DS: Got error attempting to add device temporarily" << QString::number(e);
        return;
    }

    OMdeviceInfo* dev = m_net->deviceInfo(p_bus, p_addr);

    const int id = dev->device->getId();
        // make sure the returned omcommandbuffer object is retained until we're done
    m_cmd->hold(id);

    m_cmdSent.insert(id, p_addr);
    m_cmdSentBus.insert(id, p_bus);

    m_scanDialog->totalNodes(m_cmdSent.count());
}

 /** Handles commands completed from the bus
   */

void DeviceScanner::_commandCompleted(int p_id, OMCommandBuffer *p_com) {

    qDebug() << "DS: Got response to" << p_id;

        // ignore any commands not sent by us
    if( ! m_cmdSent.contains(p_id) )
        return;

    m_respCount++;

        // update progress bar
    m_scanDialog->scannedNodes(m_respCount);



    unsigned short addr = m_cmdSent.value(p_id);
    QString bus = m_cmdSentBus.value(p_id);

        // go ahead and get rid of the device now
    m_net->deleteDevice(bus, addr, false);

        // failure - the device wasn't on the bus, or didn't
        // understand a required command

    if( p_com->status() != OMC_SUCCESS ) {
        qDebug() << "DS: Scan Node Not Found:" << bus << addr;
        m_cmd->release(p_id);
        _checkDone();
        return;
    }

    qDebug() << "DS: Scan Node Got a response:" << bus << addr;

    m_foundCount++;

        // get result of command (id string)
    int resSize = p_com->resultSize();
    char* res = new char[resSize];
    p_com->result(res, resSize);
    QString idStr = QString::fromAscii(res, resSize);
    delete res;

    QString foundMsg("Found new device on bus " + bus + " at address " + QString::number(addr) + "\t\tType = " + idStr + "\n");

    devInfo thsDevice(bus, idStr, addr);

    m_foundDevs.append(thsDevice);

    m_scanDialog->addNote(foundMsg);

    m_cmd->release(p_id);

    _checkDone();

}

/** When a scan is accepted, this slot displays an AddDeviceDialog for every device found
  */

void DeviceScanner::_scanAccepted() {
    m_scanDialog->hide();
    if( m_findAddr == 0 ) {
        foreach(devInfo dev, m_foundDevs) {
            AddDeviceDialog addDev(m_net, dev.bus, dev.id, dev.addr);
            addDev.exec();
        }
    }
    else {
        if(m_foundDevs.count() < 1) {
            ErrorDialog er;
            er.setError("Could not find a device at the default address of 2. Perhaps you should scan instead?");
            er.exec();
        }
        else {
            devInfo dev = m_foundDevs[0];
            qDebug() << "Starting DeviceAssingDialog with" << dev.bus << dev.id;
            DeviceAssignDialog assign(m_net, dev.bus, dev.id);
            assign.exec();
        }
    } // end if scanning a specific address
}

void DeviceScanner::_checkDone() {
        // are we done?
    if( m_respCount >= m_cmdSent.count() ) {
            // enable 'Done' button
        m_scanDialog->enableConfirm(true);
        if( m_foundCount <= 0 )
            m_scanDialog->addNote("No New Devices Were Found");
        else
            m_scanDialog->addNote("Found " + QString::number(m_foundCount) + " New Devices");

}
}
