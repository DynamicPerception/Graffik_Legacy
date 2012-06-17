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

    LIBS += -L../libs/qextserialport/src/build
}

macx {
    LIBS += -L../libs/OpenMoCo/build
    LIBS += -L$$(HOME)/lib
}



LIBS += -lopenmoco


win32 {
    LIBS += -lqextserialport1
}

macx {
    LIBS += -lqextserialport
    ICON = graffik.icns
}

SOURCES += main.cpp\
	   mainwindow.cpp 

HEADERS  += mainwindow.h 

FORMS    += mainwindow.ui 

 # load core widgets and other items
include(core/core.pri)
include(film/film.pri)
include(slim/slim.pri)
include(docs/docs.pri)

#RESOURCES += \
#    buttons.qrc

RESOURCES += \
    icons.qrc


win32 {
    CONFIG(debug, debug|release) {
        DEPDIR = $$OUT_PWD/debug
    }
    CONFIG(release, debug|release) {
        DEPDIR = $$OUT_PWD/release
    }
    DEPDIR ~= s,/,\\,g
}


defineTest(deployFiles) {
    files = $$1


    for(FILE, files) {
        FILE = $$FILE
        DEPFILECOPY += @echo "Copying $$FILE" $$escape_expand(\\n\\t)
        # Replace slashes in paths with backslashes for Windows
        win32:FILE ~= s,/,\\,g


        DEPFILECOPY += $$QMAKE_COPY $$quote($$FILE) $$quote($$DEPDIR) $$escape_expand(\\n\\t)
    }
    export(DEPFILECOPY)
}



win32 {

    # only copy dlls and destroy source files for release versions
 CONFIG(release, debug|release) {

DEP_FILES = \
    $$[QT_INSTALL_BINS]\\QtCore4.dll \
    $$[QT_INSTALL_BINS]\\QtGui4.dll \
    $$[QT_INSTALL_BINS]\\QtHelp4.dll \
    $$[QT_INSTALL_BINS]\\QtCLucene4.dll \
    $$[QT_INSTALL_BINS]\\QtSql4.dll \
    $$[QT_INSTALL_BINS]\\QtNetwork4.dll \
    $$OUT_PWD\\..\\libs\\qextserialport\src\build\qextserialport1.dll


    deployFiles($$DEP_FILES)

    # need to deploy needed dll's, etc. more easily

    deploy_copy.target = windep
    deploy_copy.commands = @echo "GRPRO: Copying Required DLL files for Windows" $$escape_expand(\\n\\t)
    deploy_copy.commands += $$DEPFILECOPY
#    deploy_copy.commands += del /F /Q $$DEPDIR\\*.o $$escape_expand(\\n\\t)
#    deploy_copy.commands += del /F /Q $$DEPDIR\\*.cpp $$escape_expand(\\n\\t)

    QMAKE_EXTRA_TARGETS += deploy_copy
    POST_TARGETDEPS += windep
    QMAKE_POST_LINK += $$PWD\\clean_win.bat $$DEPDIR
 }

}


