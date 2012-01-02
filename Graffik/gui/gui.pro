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
    }
    CONFIG(release, debug|release) {
        LIBS += -L../libs/OpenMoCo/release
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
           slimwindow.cpp \ 
    filmwindow.cpp

HEADERS  += mainwindow.h \
	    slimwindow.h \ 
    filmwindow.h

FORMS    += mainwindow.ui \
	    slimwindow.ui \ 
    filmwindow.ui

 # load core widgets and other items
include(core/core.pri)

#RESOURCES += \
#    buttons.qrc

RESOURCES += \
    icons.qrc
