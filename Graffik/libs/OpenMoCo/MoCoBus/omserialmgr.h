#ifndef OMSERIALMGR_H
#define OMSERIALMGR_H

#include <QQueue>
#include <QThread>
#include <QObject>
#include <QString>

#include "qextserialport.h"
#include "openmoco.h"
#include "omcommandbuffer.h"


class OMMoCoBus;


/**

  An OpenMoCo Serial Manager controls access to a particular
  QextSerialPort object, queueing commands when not connected and
  providing a mechanism for complete asynchronous communication
  with a MoCoBus device using QThread.

  This class should not be used directly, instead the OMNetwork class
  should be used to manage busses and devices easily.

  @author
  C. A. Church

  */

class OMSerialMgr : public QThread, public OpenMoCo
{
    Q_OBJECT

public:
    OMSerialMgr(QString, OMMoCoBus*);

    void connect();

    const bool isConnected();

    void run();

signals:
    void commandComplete(OMCommandBuffer*);

public slots:
    void commandAdded(OMCommandBuffer*);

private:
    QextSerialPort* _qSer;
    QQueue<OMCommandBuffer*> _cmdQueue;

    bool _connected;
    volatile bool _serWaiting;

    int _commandWaiting;

    void _processQueuedCommands();
    bool _compare(QByteArray, QByteArray, int);

    void _sendCom(OMCommandBuffer*&);
    void _getResponse(OMCommandBuffer*&);
    void _cmdErr(OMCommandBuffer*&);
    int _getSerByte();

private slots:

};

#endif // OMSERIALMGR_H
