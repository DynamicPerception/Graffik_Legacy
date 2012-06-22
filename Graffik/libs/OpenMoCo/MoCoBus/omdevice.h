#ifndef OMDEVICE_H
#define OMDEVICE_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QByteArray>


#include "openmoco.h"
#include "MoCoBus/omcommandbuffer.h"
#include "MoCoBus/ommocobus.h"

/** OpenMoCo Base Device

    This class is used as a base class for creating objects which
    can speak to and control remote devices on a MoCoBus network.

    This class is a building block with all of the core functionality
    required to communicate to your device on the MoCoBus.  Creating
    a new device type is as simple as defining the device and then
    creating your own methods which transmit the right information
    on your behalf.

    All commands to the device are sent asynchronously, using the command()
    methods provided.  Typically, the creator of the new, derived class
    will call these for the user of their class when neccessary.  Since these
    commands are sent asynchronously, to receive the result of the command
    you will need a slot to accept the signal that is emitted by this
    device.  For more information, see the information on the complete() signal
    below.

    Every command issued to a device results in the creation of an OMCommandBuffer object.
    Note that OMCommandBuffer objects created for you by the protected command()
    methods are not automatically deleted - even when this device is destroyed.
    The purpose for this is that a consumer of an OMDevice may wish to handle the
    buffers themselves - for example, reading a result and displaying it to a user,
    and as such any automatic management of the buffer object is undesirable.  Certain
    classes, such as OMNetwork, provide self-management capabilities for simple
    implementation by a consumer of your library.  It is highly suggested to add
    your new device to the OMNetwork supported types for this reason.

    Example:

    MyDev.h:
    @code

    #include "MoCoBus/omdevice.h"

    class MyDev : public OMDevice
    {
        Q_OBJECT

     public:

        MyDev(OMMoCoBus*, unsigned short);
        const int doThis();
    };
    @endcode

    MyDev.cpp:
    @code

    #include "MyDev.h"

    MyDev::MyDev(OMMoCoBus* c_bus, unsigned short c_addr) : OMDevice(c_bus, c_addr) {
        deviceType = "My Device";
        deviceVersion = 1;
    }

    const int MyDev::doThis() {
        // send a command, return command id
        return this->command(1, 1); //
    }
    @endcode

    @author
    C. A. Church
    (c) Copyright 2011-2012 Dynamic Perception LLC

    Licensed under the terms of the GNU LGPL version 3.

  */

class OMDevice : public QObject
{
    Q_OBJECT
public:

   OMDevice(OMMoCoBus*, unsigned short);
   virtual ~OMDevice();

   OMMoCoBus* bus();
   unsigned short address();
   unsigned short version();
   QString type();
   int id();   

   const int getVersion();
   const int getBusVer();
   const int getId();
   const int changeAddress(uint8_t);

protected:

   unsigned short deviceAddress;
   unsigned short deviceVersion;
   QString deviceType;
   OMMoCoBus* deviceBus;

   const int command(int, char);
   const int command(int, char, char);
   const int command(int, char, unsigned short);
   const int command(int, char, unsigned long);
   const int command(int, char, char, char);
   const int command(int, char, char, unsigned short);
   const int command(int, char, char, unsigned long);
   const int command(int, char, char, float);
   const int command(int, char, char*, int);

   char* nwo(unsigned short);
   char* nwo(unsigned long);
   char* nwo(float);

   static const char    COMCORE = 1;

   static const char coreBus       = 1;
   static const char coreVersion   = 3;
   static const char coreId        = 2;
   static const char coreAddr      = 4;

private:



    static int m_devId;
    int m_thisDevId;

private slots:

    void _rcvQueued(OMCommandBuffer*);
    void _rcvComplete(OMCommandBuffer*);

signals:

    /** Command Complete

      This signal is emitted when a command is completed by the underlying
      serial manager. An OMCommandBuffer object representing the command, including
      its result, is passed through this signal. You can determine which command was
      completed by accessing the id() method of the passed OMCommandBuffer object.
      The result values will be populated in the OMCommandBuffer object.

      A complete() signal emitted from an OMDevice is always from a command issued by that
      device.  Connecting this signal to a slot is the most specific way to handle responses
      from a particular device.  If you want to monitor all commands sent to a bus, without
      concern as to which device - then attach your slot to the complete signal from OMMoCoBus.

      The following is an example of a slot to catch the command completion:

      @code
      void commandCompleted(OMCommandBuffer* cmd) {
        qDebug() << "Received completion for command id " << cmd->id();

        if( cmd->status() == OMCommandBuffer::OMC_SUCCESS )
            qDebug() << "Command Succeeded";

        if( cmd->status() == OMCommandBuffer::OMC_FAILURE )
            qDebug() << "Command Failed";

      }
      @endcode

   */

    void complete(OMCommandBuffer*);

    /** Command Queued

      This signal is emitted when a command is queued.
      */

    void queued(OMCommandBuffer*);


};

#endif // OMDEVICE_H
