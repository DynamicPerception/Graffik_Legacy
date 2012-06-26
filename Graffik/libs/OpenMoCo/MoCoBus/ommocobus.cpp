#include <QDebug>
#include <QMetaType>

#include "ommocobus.h"


/** Constructs a new Controller

  Create a new controller, attached to a particular com port.

  @param port
  The serial port name

  */


OMMoCoBus::OMMoCoBus(QString p_port) : QObject()
{


    m_port = p_port;
    m_threadStarted = false;

    _connected = false;
    _setupSerial();

}


OMMoCoBus::~OMMoCoBus() {
        // delete the object we created

    if( m_threadStarted ) {
        m_serThread->quit();
        m_serThread->wait();
        delete m_serThread;
        delete _serMgr;
    }

}


/** Get ID

  @return
  Bus ID
  */

unsigned short OMMoCoBus::id() {
    return(_id);
}


void OMMoCoBus::_setupSerial() {

    _serMgr = new OMSerialMgr(m_port, this);
    m_serThread = new QThread;



    qRegisterMetaType<OMCommandBuffer*>("OMCommandBuffer");

        // we use a signal to communicate with the sermgr thread
    QObject::connect(this, SIGNAL(queued(OMCommandBuffer*)), _serMgr, SLOT(commandAdded(OMCommandBuffer*)), Qt::QueuedConnection);
        // .. and it uses one to communicate w/ us
    QObject::connect(_serMgr, SIGNAL(commandComplete(OMCommandBuffer*)), this, SLOT(commandCompleted(OMCommandBuffer*)), Qt::QueuedConnection);
        // reflect signal
    QObject::connect(_serMgr, SIGNAL(commandComplete(OMCommandBuffer*)), this, SIGNAL(complete(OMCommandBuffer*)), Qt::QueuedConnection);

    _serMgr->moveToThread(m_serThread);
    m_serThread->start();
    m_threadStarted = true;
}



/** Connect to the specified serial port

  Opens the configured serial device

  @throws OM_ERR_SERAVAIL
  Serial connection was unavailable.

  */

void OMMoCoBus::connect() {

    if(_connected)
        return;

    try {
        _serMgr->connect();
    }
    catch (int e) {
        throw e;
    }

    _connected = true;

}

/** Return bus port name
  */

QString OMMoCoBus::port() {
    return(m_port);
}

/** Send Broadcast Command

   Send a broadcast command to all devices on this bus

   @param p_bcmd
   The command to send to the bus

   @return
   The ID of the command issued

   The following pre-defined broadcast commands are available:

   OMBus::OM_BCAST_START
   OMBus::OM_BCAST_STOP
   OMBus::OM_BCAST_PAUSE


*/

const int OMMoCoBus::broadcast(uint8_t p_bcmd) {

    OMCommandBuffer* cmdBuf = new OMCommandBuffer(OMBus::OM_SER_BCAST_ADDR, 0);
    char* data = new char[0]();
    cmdBuf->setPayload(data, p_bcmd, 0);
    delete[] data;
    cmdBuf->broadcast(true);
    queueCommand(cmdBuf);
    return(cmdBuf->id());
}

/** Adds a command to the command queue

  @param cmdBuf
  The address of a properly created (i.e. payload set) OMCommandBuffer
  object pointer


  */

void OMMoCoBus::queueCommand(OMCommandBuffer* &cmdBuf) {
    qDebug() << "OMB: Queued command : " << QThread::currentThreadId();
    emit queued(cmdBuf);
}


void OMMoCoBus::commandCompleted(OMCommandBuffer * buf) {
    qDebug() << "OMB: Command Rcvd Back: " << (unsigned char) buf->id();
}

