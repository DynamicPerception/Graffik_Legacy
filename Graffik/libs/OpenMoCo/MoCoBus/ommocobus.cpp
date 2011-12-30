#include <QDebug>
#include <QMetaType>

#include "ommocobus.h"


 // initialize static members

/** Constructs a new Controller

  Create a new controller, attached to a particular com port.

  @param port
  The serial port name

  */


OMMoCoBus::OMMoCoBus(QString p_port) : OpenMoCo(), QObject()
{

    m_port = p_port;
    _connected = false;
    _setupSerial();

}


OMMoCoBus::~OMMoCoBus() {
        // delete the object we created
    _serMgr->quit();
    delete _serMgr;
}


/** Get ID

  @return
  Bus ID
  */

unsigned short OMMoCoBus::id() {
    return(_id);
}


void OMMoCoBus::_setupSerial() {
    qDebug() << "SerCreate";

    _serMgr = new OMSerialMgr(m_port, this);

    qDebug() << "SerCreated";


    qRegisterMetaType<OMCommandBuffer*>("OMCommandBuffer");

        // we use a signal to communicate with the sermgr thread
    QObject::connect(this, SIGNAL(queued(OMCommandBuffer*)), _serMgr, SLOT(commandAdded(OMCommandBuffer*)), Qt::QueuedConnection);
        // .. and it uses one to communicate w/ us
    QObject::connect(_serMgr, SIGNAL(commandComplete(OMCommandBuffer*)), this, SLOT(commandCompleted(OMCommandBuffer*)), Qt::QueuedConnection);
        // reflect signal
    QObject::connect(_serMgr, SIGNAL(commandComplete(OMCommandBuffer*)), this, SIGNAL(complete(OMCommandBuffer*)), Qt::QueuedConnection);
}


/** Connect to the specified serial port

  Opens the configured serial device

  @throws errSerialNotAvailable
  Serial connection was unavailable.

  */

void OMMoCoBus::connect() {


    try {
        _serMgr->connect();
    }
    catch (int e) {
        throw e;
    }

}


/** Adds a command to the command queue

  @param cmdBuf
  The address of a properly created (i.e. payload set) OMCommandBuffer
  object pointer


  */

void OMMoCoBus::queueCommand(OMCommandBuffer* &cmdBuf) {
    qDebug() << "Got command : " << _cmdQueue.count() << " " << QThread::currentThreadId();

    emit queued(cmdBuf);
}


void OMMoCoBus::commandCompleted(OMCommandBuffer * buf) {
    qDebug() << "Command Rcvd Back: " << (unsigned char) buf->id();
}

