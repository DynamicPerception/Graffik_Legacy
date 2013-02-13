PROJECT                 = mocobus
TEMPLATE                = lib
VERSION                 = 1.0.0
DESTDIR                 = ..
CONFIG                  += qt warn_on debug_and_release
CONFIG                  += staticlib
OBJECTS_DIR             = tmp
MOC_DIR                 = tmp
DEPENDDIR               = .
INCLUDEDIR              = .

INCLUDE_PATH		+= ../qextserialport/src/
LIBS 			+= -L../libs/qextserialport/src/build -lqextserialport1

HEADERS                 = omcommand.h \
			  omcommandbuffer.h \
			  omcontroller.h \
			  omserialmgr.h
			  
SOURCES                 = omcommand.cpp \
			  omcommandbuffer.cpp \
			  omcontroller.cpp \
			  omserialmgr.cpp

TARGET 			= mocobus

