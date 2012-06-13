#ifndef OPENMOCO_H
#define OPENMOCO_H


#define SLIM_ERR_EMPTY  200
#define SLIM_ERR_NOCMD   201
#define SLIM_ERR_ARG   202
#define SLIM_ERR_ARGS   203
#define SLIM_ERR_NODEV  204

/** Base OpenMoCo Class

  Defines some core data elements for use in MoCoBus libraries

  @author
  C. A. Church

  (c) 2011-2012 Dynamic Perception LLC
  Licensed under the terms of the GNU LGPL version 3.
  */


// result data types

enum {
   R_BYTE, R_UINT, R_INT, R_LONG, R_ULONG, R_FLOAT, R_STRING
};

class OpenMoCo
{
public:
    OpenMoCo();

    static const int errInvalidArgument = 2;
    static const int errOutOfMemory     = 3;
    static const int notQueued      = 4;
    static const int errSerialNotAvailable = 5;
    static const int errCommandNotFound = 6;

};

#endif // OPENMOCO_H
