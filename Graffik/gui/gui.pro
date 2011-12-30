# Graffik 
# (c) 2011 Dynamic Perception LLC
# Author: c. a. church


QT       += core gui

TARGET = Graffik
TEMPLATE = app
UI_DIR = .ui

CONFIG += ordered

INCLUDEPATH  += ../libs/qextserialport/src/ \
		../libs/OpenMoCo \
                ../libs/OpenMoCo/OpenMoCo \
                .

 # link to openmoco libraries


win32 {
    CONFIG(debug, debug|release) {
        LIBS += -L../libs/OpenMoCo/debug
 #       INCLUDEPATH += "/Program Files/Microsoft SDKs/Windows/v7.1/Include"
    }
    CONFIG(release, debug|release) {
        LIBS += -L../libs/OpenMoCo/release
#        INCLUDEPATH += "C:/Program Files/Microsoft SDKs/Windows/v7.1/Include"
    }
}

macx {
    LIBS += -L../libs/OpenMoCo/build
}


LIBS += -lopenmoco

LIBS += -L../libs/qextserialport/src/build \

win32 {
    LIBS += -lqextserialport1
}

macx {
    LIBS += -lqextserialport
}

SOURCES += main.cpp\
	   mainwindow.cpp \
           slimwindow.cpp 

HEADERS  += mainwindow.h \
	    slimwindow.h 

FORMS    += mainwindow.ui \
	    slimwindow.ui 

 # load core widgets and other items
include(core/core.pri)

#RESOURCES += \
#    buttons.qrc

RESOURCES += \
    icons.qrc
