#ifndef OMNETWORK_H
#define OMNETWORK_H

#include <QString>
#include <QList>
#include <QHash>
#include <QColor>
#include <QObject>
#include <QByteArray>

#include "openmoco.h"
#include "ommocobus.h"
#include "omdevice.h"
#include "omcommandmanager.h"

#define OM_NET_DUPE 1501
#define OM_NET_BUS  1502
#define OM_NET_ADDR 1503
#define OM_NET_DEV  1504



 /** Information about a specific device

   Stores information related to a specific device.  Note that
   The information about the bus isn't stored here - as device
   information is retrieved from the bus information.
   */
struct OMdeviceInfo {
    /** device name */
    QString name;
    /** device type string */
    QString type;
    /** count of commands issued to device */
    int commands;
    /** count of commands that resulted in an error */
    int errors;
    /** Pointer to the actual device object */
    OMDevice* device;
    /** Hash of command history */
    QHash<int, OMCommandBuffer*>* commandHistory;
};


 // port is not a member, see m_busList vector.

 /** Information about a specific bus

    Stores information related to a specific bus.  Note that
    the devices member contains a QHash with a pointer to the
    OMdeviceInfo structure for each device on this bus.
    */

struct OMbusInfo {
    /** bus name */
    QString name;
    /** bus color */
    QColor color;
    /** OMMoCoBus object pointer */
    OMMoCoBus* bus;
    /** Hash of devices - device address is key */
    QHash<unsigned short, OMdeviceInfo*> devices;
};

/** MoCoBus Network Manager

  The OMNetwork class allows you to easily create and manage one or more
  buses of devices.  This class provides all needed functionality to both
  create devices and provide a map of the current state of the network.

  OMNetwork should be the primary interface to devices and buses for the casual
  implementor who wants to attach the network model to a view, or perform other
  activities.  Only those wishing a very specific (or extremely light-weight)
  implementation should use OMMoCoBus or OMDevice directly.

  A network consists of one or more buses, each bus is connected to a unique
  serial port.  One each bus, numerous devices may live.  OMNetwork provides
  a simple way to create and manage these buses and devices as a single network.

  Rather than have to create each MoCoBus device to represent a bus, and implement
  your own way to then create devices and map them to buses, you may simply create
  an object of this type, and call a few simple methods to achieve the same end result.

  As each MoCoBus object creates its own thread for serial communication with the
  devices attached to it, traffic on one bus does not impact another bus.  Unlike
  using a MoCoBus directly, all buses are automatically connected when added via
  OMNetwork.

  The following is a basic example of creating a network with one bus and three
  devices:

  @code
  #include "MoCoBus/omnetwork.h"

  ...

  OMNetwork* net = new OMNetwork();

  net.addBus("COM5", "My Bus");

  net.addDevice("COM5", 2, "OpenMoCo Axis", "pan");
  net.addDevice("COM5", 3, "OpenMoCo Axis", "tilt");
  net.addDevice("COM5", 4, "OpenMoCo Axis", "dolly");

  @endcode

  You now have a network of three devices on one bus.  Information
  about each device or bus (including the actual bus and device objects)
  can be retrieved using deviceInfo() and busInfo().

  You may also get a list of the address of each device on a specific bus
  using getDevices(), or list all buses with getBuses().

  An additional benefit of the OMNetwork class, is that it collects all complete()
  and queued() signals from all of the buses, allowing one to route all command queued
  and complete signals from all buses through a single channel.

  @author C. A. Church

  (c) 2011-2012 Dynamic Perception LLC
  Licensed under the terms of the GNU LGPL version 3.
  */

class OMNetwork : public QObject, public OpenMoCo
{
    Q_OBJECT

public:
    explicit OMNetwork(int c_cmdHist = 0);
    ~OMNetwork();

    void addBus(QString, QString);
    bool deleteBus(QString);
    bool busExists(QString);
    void busColor(QString, QColor);
    OMbusInfo* busInfo(QString);
    QList<QString> getBuses();

    void addDevice(QString, unsigned short, QString, QString);
    void deleteDevice(QString, unsigned short);
    OMdeviceInfo* deviceInfo(QString, unsigned short);
    QList<unsigned short> getDevices(QString);
    QStringList deviceTypes();

    OMCommandManager* getManager();

    const int broadcast(uint8_t p_bcmd);

private:
        // "port" points to bus info.
    QHash<QString, OMbusInfo*> m_busList;
        // device ids to devices...
    QHash<int, OMdeviceInfo*> m_devIds;
        // list of valid device type names
    QStringList m_devTypes;

    int m_histCnt;

    OMDevice* _createDevice(OMMoCoBus*, unsigned short, QString);

    OMCommandManager* m_cmdMgr;

signals:

    /** Complete Signal

      Every OMMoCoBus object emits a complete() signal when a command has been
      completed.  Normally, one would have to connect to the signals of each and
      every MoCoBus object to get the result of commands.  However, OMNetwork
      routes all of these signals through its self, and then reflects them as
      a single signal connection point.

      Additionally, if you have command history enabled in the network, the
      OMNetwork object ensures that the command has not been purged from the
      history -at the time the signal is sent-.
      */

    void complete(OMCommandBuffer* p_cmd);

    /** Command Queued Signal

      See complete() for more info.
      */

    void queued(OMCommandBuffer* p_cmd);

    /** Bus Added Signal

      */

    void busAdded(QString p_port);
    void busAdded(OMbusInfo* p_bus);

    /** Bus deleted signal
      */

    void busDeleted(QString p_port, QString p_name);

    /** Device Added Signal

      */
    void deviceAdded(QString, unsigned short);
    void deviceAdded(OMbusInfo*, OMdeviceInfo*);
    void deviceAdded(OMdeviceInfo*);

    /** Device Deleted Signal

      */

    void deviceDeleted(QString, unsigned short);
    void deviceDeleted(OMbusInfo*, unsigned short);

private slots:
    void _queued(OMCommandBuffer*);
    void _complete(OMCommandBuffer*);

};

#endif // OMNETWORKMODEL_H
