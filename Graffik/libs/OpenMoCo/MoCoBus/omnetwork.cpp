#include "omnetwork.h"

#include <QDebug>

#include "Devices/nanoMoCo/omaxis.h"


/** Constructor

  Creates a new OMNetwork object, with a command history level
  optionally specified.

  Each OMNetwork object retains a history of every command sent to
  a device it manages, and can be used to "subscribe" to specific
  commands of interest. The number of commands which will be retained
  for each device is specified in an optional argument.

  The command history level argument specifies how many previous
  commands to keep in memory for each device, with zero (the default value)
  meanining do not retain history.

  e.g.:

  @code

  OMNetwork* netNoHist = new OMNetwork();

  OMNetwork* netHist = new OMNetwork(20);
  @endcode

  When history is enabled, you must not save a pointer to any OMCommandBuffer
  received from any device on this network, as the OMNetwork object will automatically
  clean them up as a device exceeds the set command history level.  Attempting
  to use such a pointer after the fact will lead to undesired, and likely disastrous
  behavior.  When history is enabled, always request a new copy of a command from
  the history when accessing.

  If history is not enabled, an OMCommandManager instance is used to manage command
  lifecycle instead.  You can retrieve he object using the getManager() method, and
  then use OMCommandManager::hold() and OMCommandManager::release() to manage the lifecycle
  of the command objects created.

  */

OMNetwork::OMNetwork(int c_cmdHist) : QObject()
{
    m_histCnt = c_cmdHist;

        // add nanoMoCo to the device types
    m_devTypes.append("OpenMoCo Axis");
        // map nanoMoCo id return value to device type name
    m_devTypeMap.insert("OMAXISVX", "OpenMoCo Axis");


    m_cmdMgr = new OMCommandManager(this);

    // if we are not storing command history, use the OMCommandManager to manage the lifecycle
    // of created OMCommandBuffer objects

    if( m_histCnt <= 0 ) {
        QObject::connect(this, SIGNAL(complete(OMCommandBuffer*)), m_cmdMgr, SLOT(registerCommand(OMCommandBuffer*)), Qt::QueuedConnection);
    }

}

OMNetwork::~OMNetwork() {

        // delete all objects we added to the heap
    foreach( QString thsBus, m_busList.keys() ) {

        qDebug() << "OMN: Destroying bus " << thsBus;
        deleteBus(thsBus);

    }

    delete m_cmdMgr;

}

 /** Retrieve the Command Manager

   If history is not enabled, you must manually manage the lifecycle of OMCommandBuffer using
   the OMCommandManager object.  Each instance of an OMNetwork gets its own OMCommandManager
   object.  This method allows you to retrieve a pointer to the OMCommandManager object in
   use by the network (in which it registers all commands received) so that you may hold()
   and release() the commands as-needed.

   Note: if history is enabled, any actions you perform with this command manager will be
   non-operative.

   @return
   A pointer to the OMCommandManager object in-use

   */

OMCommandManager* OMNetwork::getManager() {
    return m_cmdMgr;
}


/** Broadcast a command to all nodes on all buses

  Send a broadcast command to all nodes on all buses.

   The following pre-defined broadcast commands are available:

   OMBus::OM_BCAST_START
   OMBus::OM_BCAST_STOP
   OMBus::OM_BCAST_PAUSE

   @param p_bcmd
   The broadcast command to send

   @return
   The command ID of the last broadcast command sent
  */

const int OMNetwork::broadcast(uint8_t p_bcmd) {
    qDebug() << "OMN: Request to broadcast command" << p_bcmd;
    int i;

    foreach(QString thsBus, m_busList.keys() )
        i = m_busList[thsBus]->bus->broadcast(p_bcmd);

    return i;
}

/** Add Bus to Network

  Creates a new bus in the network, connected to the specified port.

  Immediately connects to the specified port, and will throw an error
  if that port is already in use.  Additionally, if any error is thrown
  in connection, it will throw that error back to the caller.

  @param p_port
  The name of the serial port to connect to, e.g. "\\.\COM11"

  @param p_name
  The nickname for the bus

  @throws OM_NET_DUPE
  This port has already been used for a bus

  May additionally throw any error returned by OMMoCoBus::connect()

  */

void OMNetwork::addBus(QString p_port, QString p_name) {

    qDebug() << "OMN: Bus add request for" << p_port << p_name;

        // throw an error on duplicate
    if( m_busList.contains(p_port) ) {
        qDebug() << "OMN: Bus" << p_port << "is already in use.";
        throw OM_NET_DUPE;
    }

    OMMoCoBus* bus = new OMMoCoBus(p_port);

        // attempt to connect the bus, and pass
        // on any error thrown at us
    try {
        bus->connect();
    }
    catch (int e) {
            // better not leak memory due to an error
        delete bus;
        throw e;
    }


    OMbusInfo* net = new OMbusInfo;

    net->bus = bus;
    net->name = p_name;
    net->color = QColor("blue");
    net->devices = QHash<unsigned short, OMdeviceInfo*>();

    m_busList[p_port] = net;

        // we listen in to all traffic passed to and from this bus
    QObject::connect(bus, SIGNAL(complete(OMCommandBuffer*)), this, SLOT(_complete(OMCommandBuffer*)), Qt::QueuedConnection);
    QObject::connect(bus, SIGNAL(queued(OMCommandBuffer*)), this, SLOT(_queued(OMCommandBuffer*)), Qt::QueuedConnection);

    emit busAdded(p_port);
    emit busAdded(net);

}

/** Set Color for Bus

   Sets the color for a bus, for use with UIs and other elements
   which require a color to display the bus.

   @param p_port
   A QString containing the port of the bus

   @param p_color
   A QColor representing the bus

   @throws OM_NET_BUS
   The bus port does not exist on the network
   */

void OMNetwork::busColor(QString p_port, QColor p_color) {
        // throw an error if bus port doesn't exist
    if( ! m_busList.contains(p_port) )
        throw OM_NET_BUS;

    m_busList[p_port]->color = p_color;
}

/** Delete Bus

  Deletes a bus from the system, and returns
  whether or not a bus was deleted.  If the bus has not
  been created, or has already been deleted - it returns
  false.

  This method also deletes the actual bus object created for
  the bus, thereby freeing all memory related to it.

  @return
  True if a bus was deleted, false if not

  @throws
  Throws any exception from deleteDevice()
  */

bool OMNetwork::deleteBus(QString p_port) {

    qDebug() << "OMN: Delete Bus Request for " << p_port;

    if( ! m_busList.contains(p_port) ) {
        qDebug() << "OMN: Strangely, the bus does not exist?";
        return false;
    }

        // eliminate any and all devices attached to this bus
    foreach( unsigned short addr, m_busList[p_port]->devices.keys() ) {
        deleteDevice(p_port, addr);
    }

    QString name = m_busList[p_port]->name;

    delete m_busList[p_port]->bus;

    m_busList.remove(p_port);

    emit busDeleted(p_port, name);

    return true;
}

/** Bus Exists?

  @return
  True if bus exists, false if not
  */

bool OMNetwork::busExists(QString p_port) {
    if( m_busList.contains(p_port) )
        return true;

    return false;
}

/** Bus Information

  Returns the bus information, in the form of OMbusInfo, for
  the specified bus, identified by its port.  Throws an error if the
  port has not been assigned to a bus.

  You can, for example, access the actual bus object via this method:

  @code

  OMbusInfo* info = network.busInfo("COM1");
  OMMoCoBus* bus = info->bus();
  @endcode

  @throws OM_NET_BUS
  The specified bus does not exist

  @return
  The bus information for the specified bus.
  */

OMbusInfo* OMNetwork::busInfo(QString p_port) {

    if( ! m_busList.contains(p_port) )
        throw OM_NET_BUS;

    return m_busList.value(p_port);
}

/** Add a Device to the Network

  Adds an OMDevice to the network, assigned to the bus on a particular port.

  The Device object of the specified type will be created for you automatically,
  provided the type you specify is one known to OMNetwork.

  After the device has been added to the network, you may then use deviceInfo() to
  retrieve the device information, including a pointer to the actual device object.

  If you only wish to use methods available in the OMDevice base class, then you
  may use the pointer to the device as-is.  However, if you intend to use the
  methods provided by the derived class (i.e. the actual device type), then you
  must dynamically re-cast the pointer to your derived type.  e.g.:

  @code


  network.addDevice(port, addr, "My Device Type", "pan");

  OMdeviceInfo* info = network.deviceInfo(port, addr);

  OMDevice* bdev = info->device;
  qDebug() << "Device Library Version " << bdev->version();

  MyDevType* newDev = dynamic_cast<MyDevType*> bdev;

     // always check that the dynamic cast succeeded!
  if( MyDevType != 0 ) {
    qDebug() << "Derived class method " << newDev->myMethod();
  }

  @endcode

  Please note that this method may throw one of a number of different
  exceptions should an error occur, always check for these excetptions -
  or your program may fail non-gracefully on an unexpected condition. e.g.:

  @code
  try {
    network.addDevice(port, addr, "My Device Type", "pan");
  }
  catch (int e) {
    qDebug() << "ERROR: Caught except from OMNetwork: " << e;
    exit();
  }
  @endcode

  This method, by default, will emit the deviceAdded() signals after the device
  has been added.  In some cases this may be undesireable. To prevent this from
  happening, provide the additional optional argument with a value of false.

  @param p_port
  A QString containing the port name of the bus this device resides on

  @param p_addr
  The address of the device on the bus

  @param p_type
  A QString containing a valid device type name.  See deviceTypes()

  @param p_name
  The human-readable name used to identify the device

  @param p_bcast
  Whether or not to broadcast the addition with the deviceAdded signals.  This
  is an optional argument, and defaults to true.

  @throws OM_NET_BUS
  The bus port has not been added to the network

  @throws OM_NET_DUPE
  The address is already in use on the given bus

  @throws OM_NET_DEV
  The device type is not supported by OMNetwork

  */

void OMNetwork::addDevice(QString p_port, unsigned short p_addr, QString p_type, QString p_name, bool p_bcast) {
        // no such bus?
    if( ! m_busList.contains(p_port) )
        throw OM_NET_BUS;

    QHash<unsigned short, OMdeviceInfo*>* devList = &m_busList[p_port]->devices;

        // address already used?
    if( devList->contains(p_addr) )
        throw OM_NET_DUPE;

    if(! m_devTypes.contains(p_type) )
        throw OM_NET_DEV;


    OMdeviceInfo* newDev = new OMdeviceInfo;

    newDev->commands = 0;
    newDev->errors = 0;
    newDev->name = p_name;
    newDev->type = p_type;
    newDev->device = this->_createDevice(m_busList[p_port]->bus, p_addr, p_type);
    newDev->commandHistory = new QHash<int, OMCommandBuffer*>;

    devList->insert(p_addr, newDev);

    // quick lookup for this device by id
    m_devIds[newDev->device->id()] = newDev;

        // only broadcast the new device if broadcast is enabled
    if( p_bcast == true ) {
        emit deviceAdded(p_port, p_addr);
        emit deviceAdded(m_busList[p_port], newDev);
        emit deviceAdded(newDev);
    }
}


/** Remove Device from Network

  Removes a device from the network, given its bus port and address.
  The device will no longer be valid, and the device object will be
  destroyed.

  @param p_port
  A QString containing the name of the port the device resides on.

  @param p_addr
  The address of the device to remove

  @throws OM_NET_BUS
  Bus port does not exist on network.

  @throws OM_NET_ADDR
  Address does not exist on bus
  */

void OMNetwork::deleteDevice(QString p_port, unsigned short p_addr, bool p_bcast) {

    qDebug() << "OMN: Request to delete device " << p_port << p_addr;

        // no such bus?
    if( ! m_busList.contains(p_port) )
        throw OM_NET_BUS;

        // address not there?
    if( ! m_busList[p_port]->devices.contains(p_addr) )
        throw OM_NET_ADDR;

    OMdeviceInfo* thsDev = m_busList[p_port]->devices.value(p_addr);

        // remove device from our quick look-up id map
    m_devIds.remove(thsDev->device->id());


    delete thsDev->device; // this works as desired, because OMDevice is polymorphic w/ the virt. destructor

        // purge any commandbuffer history

    foreach( int key, thsDev->commandHistory->keys() ) {
        delete thsDev->commandHistory->value(key);
    }

    delete thsDev->commandHistory;

        // remove the device information

    m_busList[p_port]->devices.remove(p_addr);

       // get rid of the device information
    delete thsDev;

    qDebug() << "OMN: Deleted device " << p_port << p_addr;

    if( p_bcast == true ) {
        emit deviceDeleted(p_port, p_addr);
        emit deviceDeleted(m_busList.value(p_port), p_addr);
    }

}

/** Device Information

  Returns the information about a given device, in the form of OMdeviceInfo.

  @param p_port
  The bus port the device resides on.

  @param p_addr
  The address of the device.

  @throws OM_NET_BUS
  The bus port has not been added to the network

  @throws OM_NET_ADDR
  The address is not in use on the bus.

  @return
  An OMdeviceInfo structure describing the device.
  */

OMdeviceInfo* OMNetwork::deviceInfo(QString p_port, unsigned short p_addr) {
        // no such bus?
    if( ! m_busList.contains(p_port) )
        throw OM_NET_BUS;


    QHash<unsigned short, OMdeviceInfo*>* devList = &m_busList[p_port]->devices;

        // address not found?
    if( ! devList->contains(p_addr) )
        throw OM_NET_ADDR;

    return devList->value(p_addr);
}


/** Get the Count of Total Buses

  Primarily intended for use by models backed by an OMNetwork,
  returns the total count of buses in the network.

  @return
  Count of buses
  */

int OMNetwork::busCount() {
    return m_busList.count();
}

/** Get the Count of Total Devices

  Primarily intended for use by models back by an OMNetwork,
  return the total count of devices in the network - across
  all buses

  @return
  Count of devices
  */

int OMNetwork::deviceCount() {
    return m_devIds.count();
}

/** Get List of all Supported Device Types

  @return
  A QStringList containing the type name for each supported device type.
  */

QStringList OMNetwork::deviceTypes() {
    return m_devTypes;
}

/** Map a Device ID String to a Supported Device Type

  Given a device ID string as returned by OMDevice::getId() to a device type
  name as accepted by OMNetwork.

  @param p_id
  A QString with the device ID

  @returns
  A QString with the valid device type name, or an empty QString if one is not found.
  */

QString OMNetwork::deviceIDtoType(QString p_id) {
    if( m_devTypeMap.contains(p_id) )
        return m_devTypeMap.value(p_id);
    else
        return QString();
}

/** Get List of Bus Ports in Network

  @return
  A QList<QString> containing the name of each bus port currently added to the network.
  */

QList<QString> OMNetwork::getBuses() {
    return m_busList.keys();
}

/** Get List of Devices on Bus

  @param p_port
  The bus port to get a list of devices for

  @return
  A QList<unsigned short> with the address of each device currently added to the bus.
  */

QList<unsigned short> OMNetwork::getDevices(QString p_port) {
    if( ! m_busList.contains(p_port) )
        return QList<unsigned short>();

    QHash<unsigned short, OMdeviceInfo*>* devList = &m_busList[p_port]->devices;

    return devList->keys();
}

/** Get List of all Devices on Network

  Returns a list of all devices on the network

  @return
  A QHash<unsigned short, OMdeviceInfo*> with every device known in the network.
  */

QHash<unsigned short, OMdeviceInfo*> OMNetwork::getDevices() {

    QHash<unsigned short, OMdeviceInfo*> retHash;

    foreach(QString bus, m_busList.keys() ) {
        QHash<unsigned short, OMdeviceInfo*>* devList = &m_busList[bus]->devices;
        foreach(unsigned short addr, devList->keys() )
            retHash.insert(addr, devList->value(addr));
    }

    return retHash;
}

 // Create a new device object - returns a pointer to the object of the specified
 // type, cast as a pointer to OMDevice base class

OMDevice* OMNetwork::_createDevice(OMMoCoBus *p_bus, unsigned short p_addr, QString p_type) {
    OMDevice *dev;

    if( p_type == "OpenMoCo Axis" ) {
        dev = new OMAxis(p_bus, p_addr);
    }


    return(dev);
}


void OMNetwork::_complete(OMCommandBuffer *buf) {

        // if a broadcast command, don't store in history,
        // but do reflect signal
    if( buf->broadcast() ) {
        emit complete(buf);
        return;
    }

    // odd, we don't have any such device known to us...

    if( ! m_devIds.contains(buf->device()) )
        return;

    OMdeviceInfo* dev = m_devIds[ buf->device() ];

    if( buf->status() == OMC_FAILURE )
        dev->errors++;

    if( m_histCnt > 0 ) {
        // need to check and make sure that we haven't exceeded
        // the requested history count, if so - purge any
        // command but this one from the history
        while( dev->commandHistory->count() > m_histCnt ) {
            int cmdId = dev->commandHistory->keys()[0];
            if( cmdId == buf->id() )
                continue;
            delete dev->commandHistory->value(cmdId);
            dev->commandHistory->remove(cmdId);
        }
    }

    emit complete(buf->id(), buf);
    emit complete(buf);

}

 // we've received a command queued signal
void OMNetwork::_queued(OMCommandBuffer *buf) {
        // odd, we don't have any such device known to us...
    if( ! m_devIds.contains(buf->device()) )
        return;

    OMdeviceInfo* dev = m_devIds[ buf->device() ];
        // increase command count
    dev->commands++;

        // add the commandbuffer to the command history for the device
    if( m_histCnt > 0 )
        dev->commandHistory->insert( buf->id(), buf );

    emit queued(buf);
}
