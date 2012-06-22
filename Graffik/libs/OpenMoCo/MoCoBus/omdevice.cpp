#include <QDebug>

#include "omdevice.h"


// initialize static members
int OMDevice::m_devId = 0;

/** Constructor

  Constructs an instance of the device, given an address for the device.

  You may not change the local device address after construction.

  (However, the core MoCoBus change address command does work, but you will
   need to remove and re-create the device with the new address afterwards.)

  @param c_bus
  An OMMoCoBus object that this device will communicate through

  @param c_addr
  The address of the device.
  */

OMDevice::OMDevice(OMMoCoBus* c_bus, unsigned short c_addr) : QObject()
{

    deviceAddress = c_addr;
    deviceBus = c_bus;

        // TODO: lock this!
    m_devId++;
    m_thisDevId = m_devId;

        // we receive signals from the bus object, so we can filter them
        // and emit a signal when a command is handled for this device
    QObject::connect(deviceBus, SIGNAL(complete(OMCommandBuffer*)), this, SLOT(_rcvComplete(OMCommandBuffer*)), Qt::QueuedConnection);
    QObject::connect(deviceBus, SIGNAL(queued(OMCommandBuffer*)), this, SLOT(_rcvQueued(OMCommandBuffer*)), Qt::QueuedConnection);

}

/** Destructor

  Virtual destructor, override in derived class to clean up any memory you
  allocate to avoid leaks.

  */

OMDevice::~OMDevice()
{

}


void OMDevice::_rcvQueued(OMCommandBuffer *p_buf) {
        // ignore commands not from us
    if( p_buf->device() != m_thisDevId )
        return;

        // reflect signal
    emit queued(p_buf);
}

void OMDevice::_rcvComplete(OMCommandBuffer *p_buf) {
        // ignore commands not from us
    if( p_buf->device() != m_thisDevId )
        return;

        // reflect signal
    emit complete(p_buf);
}


/** Get Device Address

  @return
  Device address
  */

unsigned short OMDevice::address() {
    return deviceAddress;
}

/** Get Device Version

  @return
  Device version
  */

unsigned short OMDevice::version() {
    return deviceVersion;
}

/** Get Device Type

  @return
  Device Type
  */

QString OMDevice::type() {
    return deviceType;
}



/** Change Node Address

  Immediately changes the address of the node, you must use
  the new address for this device after calling this command.

  @return
  The ID of the command
  */

const int OMDevice::changeAddress(uint8_t p_addr) {
    return this->command(COMCORE, coreAddr, (char) p_addr);
    deviceAddress = p_addr;
}

/** Retrieve Firmware Version from Node

 Retrieves the firmware version of the node.


 @return
 The ID of the command
 */

const int OMDevice::getVersion() {

   return this->command(COMCORE, coreVersion);
}

/** Retrieve Device Identifier

 Retrieves the device identifier of the given node.


 @return
 The ID of the command
 */

const int OMDevice::getId() {

   return this->command(COMCORE, coreId);
}

/** Retrieve Bus Protocol Version

 Retrieves the bus protocol version of the given node.


 @return
 The ID of the command
 */

const int OMDevice::getBusVer() {

   return this->command(COMCORE, coreBus);
}

/** Get Associated Bus Object

  @return
  OMMoCoBus object pointer
  */

OMMoCoBus* OMDevice::bus() {
    return deviceBus;
}

/** Return Device ID

  This ID is unique across all OMDevice-derived objects

  @return
  Unique Device ID
  */

int OMDevice::id() {
    return(m_thisDevId);
}


char* OMDevice::nwo(unsigned short val) {
   char* data = new char[2]();

   data[0] = (unsigned char) (val >> 8);
   data[1] = (unsigned char) val;

       // NOTE! Caller must delete[] returned pointer!
   return(data);
}

char* OMDevice::nwo(unsigned long val) {
   char* data = new char[4]();

   data[0] = (unsigned char) (val >> 24);
   data[1] = (unsigned char) (val >> 16);
   data[2] = (unsigned char) (val >> 8);
   data[3] = (unsigned char) val;

       // NOTE! Caller must delete[] returned pointer!
   return(data);
}

char* OMDevice::nwo(float val) {

   unsigned long * _tval = (unsigned long *) (&val);

   char* data = new char[4]();

   data[0] = (unsigned char) (*_tval >> 24);
   data[1] = (unsigned char) (*_tval >> 16);
   data[2] = (unsigned char) (*_tval >> 8);
   data[3] = (unsigned char) *_tval;

       // NOTE! Caller must delete[] returned pointer!
   return(data);
}


const int OMDevice::command(int maincom, char type) {

   OMCommandBuffer* cmd = new OMCommandBuffer(deviceAddress, m_thisDevId);

   char* data = new char[1]();

   if( data == NULL )
       // could not re-allocate memory
       throw(OM_ERR_MEM);



   data[0] = type;

   // _program command

   cmd->setPayload(data, maincom, 1);
   delete[] data;

       // add to controller queue
   deviceBus->queueCommand(cmd);

   return(cmd->id());
}

const int OMDevice::command(int maincom, char type, char val) {

   OMCommandBuffer* cmd = new OMCommandBuffer(deviceAddress, m_thisDevId);

   char* data = new char[2]();

   if( data == NULL )
       // could not allocate memory
       throw(OM_ERR_MEM);

   // data

   data[0] = type;
   data[1] = val;

   // _program command

   cmd->setPayload(data, maincom, 2);

   delete[] data;

   // add to controller queue
   deviceBus->queueCommand(cmd);
   return(cmd->id());
}

const int OMDevice::command(int maincom, char type, unsigned short val) {

   OMCommandBuffer* cmd = new OMCommandBuffer(deviceAddress, m_thisDevId);

   char* data = new char[3]();

   if( data == NULL )
       // could not re-allocate memory
       throw(OM_ERR_MEM);


   data[0] = type;

       // get network-order version
   char* comDat = nwo(val);

   char* dPtr = data;
   dPtr += 1;

       // set data bytes
   memcpy((void*) dPtr, comDat, 2);

   cmd->setPayload(data, maincom, 3);

   delete[] comDat;
   delete[] data;

   // add to controller queue
   deviceBus->queueCommand(cmd);
   return(cmd->id());
}

const int OMDevice::command(int maincom, char type, unsigned long val) {

   OMCommandBuffer* cmd = new OMCommandBuffer(deviceAddress, m_thisDevId);

   char* data = new char[5]();

   if( data == NULL )
       // could not re-allocate memory
       throw(OM_ERR_MEM);


   data[0] = type;

       // get network-order version
   char* comDat = nwo(val);

   char* dPtr = data;
   dPtr += 1;

       // set data bytes
   memcpy((void*) dPtr, comDat, 4);

   cmd->setPayload(data, maincom, 5);

   delete[] comDat;
   delete[] data;

   // add to controller queue
   deviceBus->queueCommand(cmd);
   return(cmd->id());
}

const int OMDevice::command(int maincom, char type, char subtype, char val) {

   OMCommandBuffer* cmd = new OMCommandBuffer(deviceAddress, m_thisDevId);

   char* data = new char[3]();

   if( data == NULL )
       // could not allocate memory
       throw(OM_ERR_MEM);

   // data

   data[0] = type;
   data[1] = subtype;
   data[2] = val;

   // _program command

   cmd->setPayload(data, maincom, 3);

   delete[] data;

   // add to controller queue
   deviceBus->queueCommand(cmd);
   return(cmd->id());
}

const int OMDevice::command(int maincom, char type, char subtype, unsigned short val) {

   OMCommandBuffer* cmd = new OMCommandBuffer(deviceAddress, m_thisDevId);

   char* data = new char[4]();

   if( data == NULL )
       // could not re-allocate memory
       throw(OM_ERR_MEM);


   data[0] = type;
   data[1] = subtype;

       // get network-order version
   char* comDat = nwo(val);

   char* dPtr = data;
   dPtr += 2;

       // set data bytes
   memcpy((void*) dPtr, comDat, 2);

   cmd->setPayload(data, maincom, 4);

   delete[] comDat;
   delete[] data;

   // add to controller queue
   deviceBus->queueCommand(cmd);
   return(cmd->id());
}

const int OMDevice::command(int maincom, char type, char subtype, unsigned long val) {

   OMCommandBuffer* cmd = new OMCommandBuffer(deviceAddress, m_thisDevId);

   char* data = new char[6]();

   if( data == NULL )
       // could not re-allocate memory
       throw(OM_ERR_MEM);


   data[0] = type;
   data[1] = subtype;

       // get network-order version
   char* comDat = nwo(val);

   char* dPtr = data;
   dPtr += 2;

       // set data bytes
   memcpy((void*) dPtr, comDat, 4);

   cmd->setPayload(data, maincom, 6);

   delete[] comDat;
   delete[] data;

   // add to controller queue
   deviceBus->queueCommand(cmd);
   return(cmd->id());

}


const int OMDevice::command(int maincom, char type, char subtype, float val) {

   char* data = new char[6]();

   if( data == NULL )
       // could not re-allocate memory
       throw(OM_ERR_MEM);


   data[0] = type;
   data[1] = subtype;

       // get network-order version
   char* comDat = nwo(val);

   char* dPtr = data;
   dPtr += 2;

       // set data bytes
   memcpy((void*) dPtr, comDat, 4);

   OMCommandBuffer* cmd = new OMCommandBuffer(deviceAddress, m_thisDevId);

   cmd->setPayload(data, maincom, 6);

   delete[] comDat;
   delete[] data;

   // add to controller queue
   deviceBus->queueCommand(cmd);
   return(cmd->id());
}

const int OMDevice::command(int maincom, char type, char* p_data, int dlen) {

   char* data = new char[dlen + 1];

   if( data == NULL )
       // could not re-allocate memory
       throw(OM_ERR_MEM);

   data[0] = type;

   char* dPtr = data;
   dPtr++;

       // set data bytes
   memcpy((void*) dPtr, p_data, dlen);

   OMCommandBuffer* cmd = new OMCommandBuffer(deviceAddress, m_thisDevId);

   cmd->setPayload(data, maincom, dlen + 1);

   delete[] data;

   // add to controller queue
   deviceBus->queueCommand(cmd);
   return(cmd->id());

}
