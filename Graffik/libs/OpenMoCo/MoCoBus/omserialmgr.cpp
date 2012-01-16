#include <QDebug>
#include <QElapsedTimer>

#include "omserialmgr.h"

/** OpenMoCo Serial Manager

  Create a new serial manager for a serial port.  Creates a new serial
  object and a thread to manage messages being sent to and from that
  serial object.

  @param port
  The serial port name

  @param parent
  A reference to the controller object that creates this serial manager

  */

OMSerialMgr::OMSerialMgr(QString port, OMMoCoBus* parent) : QThread(), OpenMoCo() {



    PortSettings settings = { BAUD57600, DATA_8, PAR_NONE, STOP_1, FLOW_OFF, 100 };
    _qSer = new QextSerialPort(port, settings, QextSerialPort::Polling);

    _connected = false;
    _serWaiting = false;

    this->start();

    qDebug() << "SerialMgr initialized in thread " << QThread::currentThreadId();

   // QObject::moveToThread(this);

}

/** Connect to serial device

  Initiates serial connection.

  @throws errSerialNotAvailable
  The serial port could not be opened.

  */

void OMSerialMgr::connect() {
    qDebug() << "Connect:";

    if(! _qSer->open(QIODevice::ReadWrite) ) {
            qDebug() << "Failed connection to serial";
            _connected = false;
            throw(errSerialNotAvailable);
    }

   // _qSer->setCommTimeouts(QSerialPort::CtScheme_NonBlockingRead);

    _connected = true;

        // commands may have been queued while disconnected
    this->_processQueuedCommands();

    qDebug() << "   Success";


}

/** Connection status

  Returns current connection status

  @return
  Connection status (true or false)

  */

const bool OMSerialMgr::isConnected() {
    return(this->_connected);
}

void OMSerialMgr::run() {

    qDebug() << "In Command Thread: " << " " << QThread::currentThreadId();

    exec();

}

/** Command Entry Slot

  Issue a command to the serial device

  @param thsCmd
  The command buffer to be sent

  */

void OMSerialMgr::commandAdded(OMCommandBuffer* thsCmd) {

    qDebug() << "In Command Thread: " << " " << QThread::currentThreadId();

    qDebug() << "Cmd Id: " << thsCmd->id();

    _cmdQueue.enqueue(thsCmd);

    qDebug() << "count: " << _cmdQueue.count();

    this->_processQueuedCommands();


}

void OMSerialMgr::_processQueuedCommands() {

    if ( this->isConnected() ) {

            // send all waiting commands
        while( ! _cmdQueue.isEmpty() ) {

            qDebug() << "Cmd Processed in " << QThread::currentThreadId();

            OMCommandBuffer* thsCmd = _cmdQueue.dequeue();
            _sendCom(thsCmd);


        }
    }

}


void OMSerialMgr::_sendCom(OMCommandBuffer* &thsCmd) {

    qDebug() << "in sendCom";

    unsigned int hdrSz = thsCmd->headerSize();
    unsigned int paySz = thsCmd->payloadSize();

    qDebug() << "serCmd " << paySz;

    char* hdr = (char*) calloc(hdrSz, sizeof(char));
    char* pay = (char*) calloc(paySz, sizeof(char));



    thsCmd->header(hdr, hdrSz);
    thsCmd->payload(pay, paySz);

    qDebug() << "Header: ";

    for( unsigned int i = 0; i < hdrSz; i++) {
        qDebug() << "   " << (unsigned char) hdr[i];
    }


    qDebug() << "Payload: " << pay;

    for( unsigned int i = 0; i < paySz; i++) {
        qDebug() << "   " << (unsigned char) pay[i];
    }

        // send serial data

    _qSer->write(hdr, hdrSz);
    _qSer->write(pay, paySz);

        // dont forget

    free(hdr);
    free(pay);

        // response handling
    this->_getResponse(thsCmd);

}

int OMSerialMgr::_getSerByte() {
    static int bytesWaiting = 0;
    QByteArray data;

    if( bytesWaiting > 0 ) {
            // we know there are bytes waiting, so don't bother
            // going back asking for how much more, just get the data
        data = _qSer->read(1);
        if( data.length() < 1 ) {
                // it wasn't there? start back over at the beginning
            qDebug() << "Failed to find expected byte?";
            bytesWaiting = 0;
        }
        else {
                // reduce counter and return byte
            qDebug() << "Returning byte from buffer w/o wait";
           bytesWaiting--;

           return(data[0]);
        }
    }

        // a hack because waitForReadyRead() is unimplemented in
        // QextSerialPort on windows, and we're not going to use
        // two different serial libraries (i.e. QSerialPort) to
        // get both enumeration AND waitForReadyRead()



        // do nothing while no bytes are ready,
        // and wait no longer than TIMEOUT ms for bytes to be ready

    QElapsedTimer timer;
    timer.start();

    while(_qSer->bytesAvailable() <= 0) {
        if( timer.elapsed() > OM_SER_TIMEOUT )
            break;
    }


    bytesWaiting = _qSer->bytesAvailable();

    if( bytesWaiting > 0 ) {
            // told there is data ready to read, in time

        qDebug() << "Bytes avilable in serial buffer: " << bytesWaiting;

            // get byte
        data = _qSer->read(1);

        bytesWaiting--;

        if( data.length() < 1 ) {
                // now that's odd...
            qDebug() << "_getSerByte() expected at least a byte, got nothing.";
            return(-300);
        }

        qDebug() << "Returning byte from buffer w/ wait";

        return(data[0]);
    }
    else {
            // didn't get any data, and none was available from a previous
            // check...
        qDebug() << "_getSerByte() timeout reading byte";
        return(-300);
    }


}


void OMSerialMgr::_cmdErr(OMCommandBuffer* &thsCmd) {
    thsCmd->status(OMC_FAILURE);
    emit commandComplete(thsCmd);
}

void OMSerialMgr::_getResponse(OMCommandBuffer* &thsCmd) {

      int curCh = _getSerByte();

      if( curCh == -300 ) {
          qDebug() << "Timeout waiting for serial response";
          _cmdErr(thsCmd);
          return;
      }


        unsigned char leadingZeros  = 0;

            // look for five zeros
        while( leadingZeros < 5 ) {

               if( curCh == 0 ) {
                    leadingZeros++;
                    qDebug() << "Got " << (unsigned char) leadingZeros << " leading zeroes";
               }
               else if( curCh == -300 ){
                   qDebug() << "Timeout reading start sequence [1]";
                   _cmdErr(thsCmd);
                   return;
               }
               else {
                    qDebug() << "Start sequence interrupted or not found, got: " << curCh;
                    _cmdErr(thsCmd);
                    return;
              }

              if( leadingZeros < 5 )
                  curCh = _getSerByte();

        }

        curCh = _getSerByte();

        if( curCh == -300 ) {
            qDebug() << "Timeout reading start sequence [2]";
            _cmdErr(thsCmd);
            return;
        }

        if( (unsigned char) curCh == 255 ) {
            qDebug() << "Found end code";
        }
        else {
            qDebug() << "Start sequence complete not found, got: " << curCh;
            _cmdErr(thsCmd);
            return;

        }

            // get address

        unsigned char addrFound = 0;
        unsigned char addr[2] = {0,0};

            // address is 2 bytes

        while( addrFound < 2 ) {

            curCh = _getSerByte();

            if( curCh == -300 ) {
                qDebug() << "Timeout reading address";
                _cmdErr(thsCmd);
                return;
            }
            else {
                addr[addrFound] = (unsigned char) curCh;
                addrFound++;
            }

        }



        if( (unsigned char) addr[0] != (unsigned char) 0 || (unsigned char) addr[1] != (unsigned char) 0 ) {
            qDebug() << "error: Response Not intended for us";
            _cmdErr(thsCmd);
            return;
        }



            // get response code

       curCh = _getSerByte();

        if( curCh == -300 ) {
            qDebug() << "Timeout reading command status";
           _cmdErr(thsCmd);
            return;
        }

            // set proper status
        if( curCh == 0 ) {
            thsCmd->status(OMC_FAILURE);
            qDebug() << "Command failed from engine, got: " << (unsigned char) curCh;
        }
        else {
            thsCmd->status(OMC_SUCCESS);
        }


        // get DLB;

        curCh = _getSerByte();

        if( curCh == -300 ) {
            qDebug() << "Timeout reading DLB";
            _cmdErr(thsCmd);
            return;
        }

        qDebug() << "Data Remaining: " << (unsigned char) curCh;

            // do we have more data to read?
        if( (unsigned char) curCh > 0 ) {

            qDebug() << "Data: ";

            int dlb = (unsigned char) curCh;
            char* data = new char[dlb - 1]();

                // get each response data byte
            for( int i = 0; i < dlb; i++) {

                qDebug() << "Response byte " << i;

                curCh = _getSerByte();
                if( curCh == -300 ) {
                    qDebug() << "Timeout reading data";
                    _cmdErr(thsCmd);
                    return;
                }

                if( i == 0 ) {
                    // data response type
                    thsCmd->setResultType(curCh);
                    qDebug() << "   " << (unsigned char) curCh << "(res type)";
                }
                else {
                        // actual data of the response
                    data[i - 1] = (unsigned char) curCh;
                    qDebug() << "   " << (unsigned char) data[i - 1];
                }

            } // end for

            thsCmd->setResult(data, dlb - 1);
            delete[] data;

        }




        // inform that command is completed
     qDebug() << "Command successfully received";

     emit commandComplete(thsCmd);

}



bool OMSerialMgr::_compare(QByteArray a, QByteArray b, int len) {
    for( int i = 0; i < len; i++ ) {
        if( a[i] != b[i] )
            return false;
    }

    return true;
}
