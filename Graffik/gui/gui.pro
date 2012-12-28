# Graffik GUI
# (c) 2011-2012 Dynamic Perception LLC
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


 # set software version string

DEFINES += "GRAFFIK_VERSION=1.013"


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

 # link to serialport lib, add icons

win32 {
    LIBS += -lqextserialport1
        # set icon
    RC_FILE = Graffik.rc
}

macx {
    LIBS += -lqextserialport
        # set icon
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



# load resources

RESOURCES += \
    icons.qrc \
    uiImages.qrc


# WIN32-Specific Build Instructions

win32 {
    CONFIG(debug, debug|release) {
        DEPDIR = $$OUT_PWD/debug
    }
    CONFIG(release, debug|release) {
        DEPDIR = $$OUT_PWD/release
    }
    DEPDIR ~= s,/,\\,g



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





    # only copy dlls and destroy source files for release versions
 CONFIG(release, debug|release) {

DEP_FILES = \
    $$[QT_INSTALL_BINS]\\QtCore4.dll \
    $$[QT_INSTALL_BINS]\\QtGui4.dll \
    $$[QT_INSTALL_BINS]\\QtHelp4.dll \
    $$[QT_INSTALL_BINS]\\QtCLucene4.dll \
    $$[QT_INSTALL_BINS]\\QtSql4.dll \
    $$[QT_INSTALL_BINS]\\QtNetwork4.dll \
    $$[QT_INSTALL_BINS]\\mingwm10.dll \
    $$[QT_INSTALL_BINS]\\libgcc_s_dw2-1.dll \
    $$OUT_PWD\\..\\libs\\qextserialport\\src\\build\\qextserialport1.dll


    deployFiles($$DEP_FILES)

    # need to deploy needed dll's, etc. more easily

    deploy_copy.target = windep
    deploy_copy.commands = @echo "GRPRO: Copying Required DLL files for Windows" $$escape_expand(\\n\\t)
    deploy_copy.commands += $$DEPFILECOPY

    QMAKE_EXTRA_TARGETS += deploy_copy
    POST_TARGETDEPS += windep
    QMAKE_POST_LINK += $$PWD\\clean_win.bat $$DEPDIR
 }

}

# END WIN32-Specific Build Instructions

# OSX-Specific Build Instructions

macx {

 CONFIG(release, debug|release) {

    QMAKE_POST_LINK += @echo "GRPRO: Deploying Mac app" $$escape_expand(\\n\\t)
    QMAKE_POST_LINK += $$[QT_INSTALL_BINS]/macdeployqt $$OUT_PWD/Graffik.app $$escape_expand(\\n\\t)
    QMAKE_POST_LINK += @echo "GRPRO: Copying qextserialport" $$escape_expand(\\n\\t)
    QMAKE_POST_LINK += cp -R $$(HOME)/lib/libqextserialport* $$OUT_PWD/Graffik.app/Contents/Frameworks/ $$escape_expand(\\n\\t)
    QMAKE_POST_LINK += @echo "GRPRO: Correcting paths inside of libqextserialport" $$escape_expand(\\n\\t)
    QMAKE_POST_LINK += install_name_tool -change $$[QT_INSTALL_LIBS]/QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore $$OUT_PWD/Graffik.app/Contents/Frameworks/libqextserialport.1.2.0.dylib $$escape_expand(\\n\\t)
#    QMAKE_POST_LINK += @echo "GRPRO: Cleaning Ojbect and Source files" $$escape_expand(\\n\\t)
#    QMAKE_POST_LINK += rm -rf $$OUT_PWD/*.o $$escape_expand(\\n\\t)
#    QMAKE_POST_LINK += rm -rf $$OUT_PWD/*.cpp $$escape_expand(\\n\\t)
 }
}

# END OSX-Specific Build Instructions
