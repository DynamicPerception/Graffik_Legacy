PROJECT                 = nanomoco
TEMPLATE                = lib
VERSION                 = 1.0.0
DESTDIR                 = ../../
CONFIG                  += qt warn_on debug_and_release
CONFIG                  += staticlib
OBJECTS_DIR             = tmp
MOC_DIR                 = tmp
DEPENDDIR               = .
INCLUDEDIR              = .
HEADERS                 = omaxis.h 
SOURCES                 = omaxis.cpp
TARGET 			= nanomoco

