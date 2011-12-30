#ifndef OMMOCOBUS_H
#define OMMOCOBUS_H

#include <QQueue>
#include <QThread>
#include <QObject>
#include <QMetaType>
#include <QColor>
#include <QString>

#include "openmoco.h"
#include "omserialmgr.h"


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
        if( e == OpenMoCo::errSerialNotAvailable ) {
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

 */


class OMMoCoBus :  public QObject, public OpenMoCo
{


    Q_OBJECT

public:
    explicit OMMoCoBus(QString);
    ~OMMoCoBus();

    OMMoCoBus(const OMMoCoBus&) : QObject() {};
    void queueCommand(OMCommandBuffer* &cmdBuf);

    unsigned short id();

    void connect();

signals:
    void queued(OMCommandBuffer*);
    void complete(OMCommandBuffer*);

public slots:
    void commandCompleted(OMCommandBuffer*);


private:

    void _setupSerial();

    unsigned short _id;

    OMSerialMgr* _serMgr;

        // we use the QT QQueue for its thread-safe FIFO
    QQueue<OMCommandBuffer*> _cmdQueue;

    bool _connected;

    QString m_port;

};




#endif // OMCONTROLLER_H
