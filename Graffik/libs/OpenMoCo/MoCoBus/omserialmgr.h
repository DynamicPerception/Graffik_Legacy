/*
    Dynamic Perception MoCoBus Development Library
    Copyright (c) 2010-2012 Dynamic Perception LLC

    This file is part of The Dynamic Perception MoCoBus Development Library.

    The Dynamic Perception MoCoBus Development Library is free software:
    you can redistribute it and/or modify it under the terms of the GNU
    Lesser General Public License as published by the Free Software Foundation,
    either version 3 of the License, or (at your option) any later version.

    The Dynamic Perception MoCoBus Development Library is distributed in the
    hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
    See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with The Dynamic Perception MoCoBus Development Library.  If not,
    see <http://www.gnu.org/licenses/>.

  */

#ifndef OMSERIALMGR_H
#define OMSERIALMGR_H

#include <QQueue>
#include <QThread>
#include <QObject>
#include <QString>

#include "qextserialport.h"
#include "openmoco.h"
#include "omcommandbuffer.h"


#define OM_SER_TIMEOUT  100

 // forward declaration

class OMMoCoBus;


/**

  An OpenMoCo Serial Manager controls access to a particular
  QextSerialPort object, queueing commands when not connected and
  providing a mechanism for complete asynchronous communication
  with a MoCoBus device using QThread.

  This class should not be used directly, instead the OMNetwork class
  should be used to manage buses and devices easily.

  @author
  C. A. Church

  (c) 2011-2012 Dynamic Perception LLC
  Licensed under the terms of the GNU LGPL version 3.

  */

class OMSerialMgr : public QObject
{
    Q_OBJECT

public:
    OMSerialMgr(QString, OMMoCoBus*);

    void connect();

    const bool isConnected();

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
    void _cmdErr(OMCommandBuffer*&, int p_err = OMC_FAILURE);
    int _getSerByte();

private slots:

};

#endif // OMSERIALMGR_H
