#ifndef OMMOCOBUS_H
#define OMMOCOBUS_H


#include <QObject>
#include <QMetaType>
#include <QThread>
#include <QString>

#include "openmoco.h"
#include "omserialmgr.h"


/** Data Types as returned by MoCoBus Commands
  */

enum {
   R_BYTE, R_UINT, R_INT, R_LONG, R_ULONG, R_FLOAT, R_STRING
};



 /**

   Defines common MoCoBus packet data values
   */

namespace OMBus {
    const uint8_t OM_SER_BCAST_ADDR = 1;
    const uint8_t OM_BCAST_START = 1;
    const uint8_t OM_BCAST_STOP = 2;
    const uint8_t OM_BCAST_PAUSE = 3;
}

class OMCommandBuffer;


/**

  An OMMoCoBus provides the ability to control devices connected
  to a specific serial port. You must first create an OMMoCoBus before
  attempting to create any OMDevice objects.

  After you have created the bus, you may perform the actual
  connection to the serial port at any time, using the connect() method.

  It is not necessary to create your own MoCoBus object if you are using the
  OMNetwork class - it will create and manage bus objects for you using
  OMNetwork::addBus(), and you may retrieve information via OMNetwork::busInfo().

  If you create an OMDevice object and issue commands before calling the connect()
  method, your commands will be queued and issued immediately upon connection.

  The following provides an example of creating and using an OMMoCoBus object:

  @code

  #include <QDebug>
  #include <QString>

  #include "MoCoBus/ommocobus.h"

  ...

  void checkBus() {

    QString serPort = "COM5";

    OMMoCoBus* bus = new OMMoCoBus(serPort, "My Bus");

    try {

        bus->connect();
    }
    catch (int e) {
        if( e == OM_ERR_SERAVAIL ) {
            qDebug() << "ERROR: Could not connect to serial port";
        }
        else {

            qDebug() << "ERROR: Caught signal: " << e;
        }

        exit();
    }

    qDebug() << "Successfully connected to " << serPort;

  }

  @endcode

  @author
  C. A. Church

  (c) 2011-2012 Dynamic Perception LLC
  Licensed under the terms of the GNU LGPL version 3.
 */


class OMMoCoBus :  public QObject
{


    Q_OBJECT

public:
    explicit OMMoCoBus(QString);
    ~OMMoCoBus();

    OMMoCoBus(const OMMoCoBus&) : QObject() {};
    void queueCommand(OMCommandBuffer* &cmdBuf);

    const int broadcast(uint8_t p_bcmd);

    unsigned short id();

    void connect();
    QString port();

signals:
    void queued(OMCommandBuffer* p_buf);
    void complete(OMCommandBuffer* p_buf);

public slots:
    void commandCompleted(OMCommandBuffer*);


private:

    void _setupSerial();

    unsigned short _id;

    OMSerialMgr* _serMgr;

    bool _connected;

    bool m_threadStarted;

    QString m_port;

    QThread* m_serThread;

};




#endif // OMCONTROLLER_H
