#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <QDebug>

#include "omcommandbuffer.h"


int OMCommandBuffer::_cmdIdTot = 0;


/** A char buffer containing the elements of an OMTLE Command

  Create a new instance of the OMCommandBuffer object, holding
  information about a MoCoBus command for, such that it can be sent
  to a communication handler.

  Additionally, this buffer will contain the response and any other
  information returned by the device, which can be used to check the
  result of a command, and so forth.

  @param c_addr
  Address of the device which we will be speaking to

  @param c_devid
  The unique device ID which we will be speaking to

  */

OMCommandBuffer::OMCommandBuffer(unsigned short c_addr, int c_devid) {
        // initialize buffer for header
    cmdHead = (char*) calloc(8, sizeof(char));

    _header(cmdHead, c_addr);

        // initialize buffer to minimum size
        // (buffer contents are set later)
    cmdBuf  = (char*) calloc(minBufSize, sizeof(char));
    bufSize = minBufSize;

        // we have to allocate something at this buffer,
        // but we start with a result size of 0 to prevent
        // false return data being reported.

    _returnDat = (char*) calloc(minBufSize, sizeof(char));
    _retSize = 0;



       //_mutex->lock();

       _cmdIdTot++;
       _myCmd = _cmdIdTot;

      // _mutex->unlock();

    _devId = c_devid;
    _execStatus = OMC_NOCMD;

    _isBCast = false;

}

OMCommandBuffer::~OMCommandBuffer() {

 free(cmdBuf);
 free(cmdHead);
 free(_returnDat);
}

/** Command Status

  Returns the status for this command

  @return
  OMC_NOCMD, OMC_QUEUED, OMC_SUCCESS, OMC_FAILURE

  */

const int OMCommandBuffer::status() {
    return this->_execStatus;
}


/** Broadcast Command?

   Is this command a broadcast command?

   @return
   True if command is a broadcast command, false otherwise
   */

bool OMCommandBuffer::broadcast() {
    return _isBCast;
}

/** Set Broadcast

   Sets whether or not this command is a broadcast command.

   @param p_bcast
   True if command is a broadcast command, false if not
   */

void OMCommandBuffer::broadcast(bool p_bcast) {
    _isBCast = p_bcast;
}

/** Command ID

  Returns the unique ID for this command

  */

const int OMCommandBuffer::id() {
    return this->_myCmd;
}

/** Device ID

  Returns the unique ID of the device this command was
  issued to.

  */

const int OMCommandBuffer::device() {
    return this->_devId;
}

/** Retrieves the header section of the command

  Copies the header of the command sequence into a specified
  character buffer.  You must specify a length of bytes to be
  copied, and if you specify fewer bytes than the header has,
  the specified length of bytes from your buffer will be over-
  written with the matching bytes from the header section.


  @param rdBuf
  A charcter pointer, with enough memory allocated to hold the
  specified number of bytes.

  @param len
  The length of bytes of the header to copy into the buffer

  */

void OMCommandBuffer::header(char* &rdBuf, int len) {

        // maximum header size is 8 bytes
    len = len > 8 ? 8 : len;

    void* ptr = memcpy(rdBuf, cmdHead, len);

    if( ptr == NULL )
        throw(OM_ERR_MEM);

}


/** Retrieves the payload section of the command.

  Copies the payload of the command sequence into a specified
  character buffer.  You must specify a length of bytes to be
  copied, and if you specify fewer bytes than the payload has,
  the specified length of bytes from your buffer will be over-
  written with the matching bytes from the payload section.


  @param rdBuf
  A charcter pointer, with enough memory allocated to hold the
  specified number of bytes.

  @param len
  The length of bytes of the payload to overwrite in the buffer

  */

void OMCommandBuffer::payload(char* &rdBuf, int len) {
        // do not attempt to copy more bytes than the payload
        // buffer holds
    len = len > bufSize ? bufSize : len;

    void* ptr = memcpy(rdBuf, cmdBuf, len);

    if( ptr == NULL )
        throw(OM_ERR_MEM);


}

/** Retrieves the result of the command.

  Copies the data resulting from the command sequence into a specified
  character buffer.  You must specify a length of bytes to be
  copied, and if you specify fewer bytes than the payload has,
  the specified length of bytes from your buffer will be over-
  written with the matching bytes from the payload section.  This
  method allows you to retrieve a response from the node which
  executed the command, if any.  Remember to always check the value
  of resultSize() before passing a length to this method.


  @param rdBuf
  A charcter pointer, with enough memory allocated to hold the
  specified number of bytes.

  @param len
  The length of bytes of the result to overwrite in the buffer

  */
void OMCommandBuffer::result(char* &rdBuf, int len) {
    len = len > _retSize ? _retSize : len;

    void* ptr = memcpy(rdBuf, _returnDat, len);

    if( ptr == NULL )
        throw(OM_ERR_MEM);

}

/** Set Command Status

  Sets the status code for the current command

  */

void OMCommandBuffer::status(int stat) {
    _execStatus = stat;
}


/** Retrieves the size of the payload.

  Retrieves the size, in bytes, of the payload.

  @return the size of the payload, in bytes.

  */

unsigned int OMCommandBuffer::payloadSize() {
    return(bufSize);

}


/** Retrieves the size of the header.

  Retrieves the size, in bytes, of the header.

  @return the size of the header, in bytes.

  */

unsigned int OMCommandBuffer::headerSize() {
    return(hdrSize);

}

/** Retrieves the size of the response

  Retrieves the size, in bytes, of the response sent to
  this command.

  @return the size of the response, in bytes.

  */

unsigned int OMCommandBuffer::resultSize() {
    return(_retSize);

}


/** Specify the payload contents.

  Adds the command payload to the object.

  @param buf
  Payload data

  @param cmd
  Primary command

  @param len
  Command Data Length

  */

void OMCommandBuffer::setPayload(char* &buf, unsigned char cmd, unsigned char len) {
    this->_initBuffer(cmd, len);


    // the command and DLB are added to cmdBuf above, so offset by that position

    char* cPtr = cmdBuf;
    cPtr += 2;

    void* ptr = memcpy((void*) cPtr, buf, len * sizeof(char));

    if( ptr == NULL )
        throw(OM_ERR_MEM);


}



/** Specify the response contents.

  Adds the response to the command the object.  This
  method is not intended to be used in normal use, but is
  instead present to allow the serial manager to place
  results in the given job.

  @param buf
  Response data

  @param len
  Response Data Length

  */

void OMCommandBuffer::setResult(char* &buf, unsigned char len) {

    free(_returnDat);

    _returnDat = (char*) calloc(len, sizeof(char));
    memset(_returnDat,0,len);

    _retSize = len;

    char* cPtr = _returnDat;

    void* ptr = memcpy((void*) cPtr, buf, len * sizeof(char));

    if( ptr == NULL )
        throw(OM_ERR_MEM);

}

void OMCommandBuffer::setResultType(unsigned char typ) {
    _retType = typ;
}

int OMCommandBuffer::resultType() {
    return(_retType);
}

void OMCommandBuffer::_initBuffer(unsigned char cmd, unsigned char size) {
        // initialize command buffer to size and set to all zeroes
        // we add two bytes for the command and DLB
    free(cmdBuf);
    cmdBuf = (char*) calloc((size + 2), sizeof(char));

    //buff = (char*) realloc( buff, (size + 2) * sizeof(char) );

    if( cmdBuf == NULL )
        // could not re-allocate memory
        throw(OM_ERR_MEM);

    memset(cmdBuf,0,size);

    bufSize = size + 2;

    cmdBuf[0] = cmd;
    cmdBuf[1] = size;
}

void OMCommandBuffer::_header(char* &buffer, unsigned short addr) {
    // prepare header
    char hdr[8] = {0,0,0,0,0,255,0,0};
    // add address to header
    hdr[6] = (unsigned char) (addr >> 8);
    hdr[7] = (unsigned char) addr;

    memcpy(buffer, hdr, 8);
}
