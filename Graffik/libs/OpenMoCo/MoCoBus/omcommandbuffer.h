#ifndef OMCOMMANDBUFFER_H
#define OMCOMMANDBUFFER_H

#include <QMutex>

#include "openmoco.h"



/** Create an object representing a MoCoBus command

  Provides requisite methods for packaging and transporting a command to be
  sent to a bus (or read from a bus) and managed through the bus queue.

  An OMCommandBuffer Object represents the contents and state of an OpenMoCo
  engine command.  The primary means for interacting with an OMCommandBuffer
  is to have received one from the signal emitted by an OMDevice object. The primary
  methods you should utilize when handling these responses are:

  id(), result(), resultSize(), status()

  Do not create an OMCommandBuffer directly, it will be created as needed for
  you through using an OMDevice.


  @author C. A. Church

  (c) 2011-2012 Dynamic Perception LLC
  Licensed under the terms of the GNU LGPL version 3.

  */

class OMCommandBuffer
{
public:
    OMCommandBuffer(unsigned short, int);

    ~OMCommandBuffer();

    void header(char* &, int);
    unsigned int headerSize();

    void payload(char* &, int);
    void setPayload(char* &, unsigned char, unsigned char);
    unsigned int payloadSize();

    void result(char* &, int);
    void setResult(char* &, unsigned char);
    void setResultType(unsigned char);
    int resultType();
    unsigned int resultSize();

    const int status();
    void status(int);

    const int id();

    const int device();

    bool broadcast();
    void broadcast(bool p_bcast);




private:

    int _myCmd;
    int _devId;

    static const int minBufSize = 1;

    mutable QMutex* _mutex;

    static int _cmdIdTot;

    char*   cmdHead;
    char*   cmdBuf;
    int     bufSize;
    char*   _returnDat;
    int     _retSize;
    int     _execStatus;
    int     _id;
    int     _retType;
    bool    _isBCast;

        // header is fixed size
    static const int hdrSize = 8;

    void   _header(char* &, unsigned short);
    void   _initBuffer(unsigned char, unsigned char);

};

#endif // OMCOMMANDBUFFER_H
