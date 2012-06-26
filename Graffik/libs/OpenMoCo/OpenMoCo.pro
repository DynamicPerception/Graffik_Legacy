#-------------------------------------------------
#
# OpenMoCo Qt Libraries
#
#-------------------------------------------------

PROJECT                 = openmoco
TEMPLATE                = lib
VERSION                 = 1.0.0

DEFINES += QT_USE_QSTRINGBUILDER

 # enable debugging in OM serial manager?
 # comment this line out to disable, then make clean and run qmake
# DEFINES += OM_SER_DBG

win32 {
    DESTDIR             = .
}

macx {
    DESTDIR     = build
}

CONFIG                  += qt warn_on debug_and_release
CONFIG                  += staticlib
OBJECTS_DIR             = tmp
MOC_DIR                 = tmp

INCLUDEPATH		+= ../qextserialport/src/


LIBS 			+= -L../qextserialport/src/build \
                           -lqextserialport1

HEADERS                 = openmoco.h \
			  MoCoBus/omcommandbuffer.h \
                          MoCoBus/omserialmgr.h \
                          Slim/slimcommandparser.h \
    MoCoBus/ommocobus.h \
    MoCoBus/omdevice.h \
    Devices/nanoMoCo/omaxis.h \
    MoCoBus/omnetwork.h \
    Slim/slimscriptdevice.h \
    MoCoBus/omcommandmanager.h
			  
SOURCES                 = \
			  MoCoBus/omcommandbuffer.cpp \
                          MoCoBus/omserialmgr.cpp \
                          Slim/slimcommandparser.cpp \
    MoCoBus/ommocobus.cpp \
    MoCoBus/omdevice.cpp \
    Devices/nanoMoCo/omaxis.cpp \
    MoCoBus/omnetwork.cpp \
    Slim/slimscriptdevice.cpp \
    MoCoBus/omcommandmanager.cpp

TARGET 			= openmoco


