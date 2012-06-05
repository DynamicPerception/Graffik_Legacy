#ifndef OPENMOCO_H
#define OPENMOCO_H


#define SLIM_ERR_EMPTY  200
#define SLIM_ERR_NOCMD   201
#define SLIM_ERR_ARG   202
#define SLIM_ERR_ARGS   203
#define SLIM_ERR_NODEV  204

    // bring about the uint8_t under windows due to msvc

#ifdef Q_WS_WIN
typedef unsigned char     uint8_t;
#endif

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
