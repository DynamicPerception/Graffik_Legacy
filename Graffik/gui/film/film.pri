DEFINES *= QT_USE_QSTRINGBUILDER
# DEFINES *= QT_NO_DEBUG_OUTPUT

CONFIG(release, debug|release) {
    # un-comment to disable all debugging output from libraries

    DEFINES *= QT_NO_DEBUG_OUTPUT
}

HEADERS += \
    film/filmwindow.h \
    film/JogControlManager/livedevicemodel.h \
    film/JogControlManager/speedcontrolproxy.h \
    film/FilmGlobalOptions/filmglobaloptions.h \
    film/JogControlManager/jogcontrolmanager.h \
    film/FilmParameters/filmparameters.h \
    film/MotionArea/motionarea.h \
    film/MotionArea/motionbase.h \
    film/Dialogs/cameracontroldialog.h \
    film/FilmExec/filmexec.h \
    film/FilmExec/positionmonitor.h \
    film/MotionArea/motionpathpainter.h \
    film/MotionArea/motiontape.h \
    film/FilmExec/playmonitor.h \
    film/MotionArea/motionsection.h \
    film/MotionArea/sectionresizefilter.h \
    film/JogControlManager/jogcontrolpanel.h \
    film/FilmFileHandler/filmfilehandler.h \
    film/FilmFileHandler/addressnotfounddialog.h \
    film/Dialogs/trackinfodialog.h \
    film/MotionArea/ErrorConsole/motionerrordialog.h \
    film/MotionArea/ErrorConsole/motionerrorelement.h \
    film/MotionArea/notrackswidget.h \
    film/FilmExec/filmtimemanager.h


SOURCES += \
    film/filmwindow.cpp \
    film/JogControlManager/livedevicemodel.cpp \
    film/JogControlManager/speedcontrolproxy.cpp \
    film/FilmGlobalOptions/filmglobaloptions.cpp \
    film/JogControlManager/jogcontrolmanager.cpp \
    film/FilmParameters/filmparameters.cpp \
    film/MotionArea/motionarea.cpp \
    film/MotionArea/motionbase.cpp \
    film/Dialogs/cameracontroldialog.cpp \
    film/FilmExec/filmexec.cpp \
    film/FilmExec/positionmonitor.cpp \
    film/MotionArea/motionpathpainter.cpp \
    film/MotionArea/motiontape.cpp \
    film/FilmExec/playmonitor.cpp \
    film/MotionArea/motionsection.cpp \
    film/MotionArea/sectionresizefilter.cpp \
    film/JogControlManager/jogcontrolpanel.cpp \
    film/FilmFileHandler/filmfilehandler.cpp \
    film/FilmFileHandler/addressnotfounddialog.cpp \
    film/Dialogs/trackinfodialog.cpp \
    film/MotionArea/ErrorConsole/motionerrordialog.cpp \
    film/MotionArea/ErrorConsole/motionerrorelement.cpp \
    film/MotionArea/notrackswidget.cpp \
    film/FilmExec/filmtimemanager.cpp

FORMS += \
    film/filmwindow.ui \
    film/MotionArea/motionarea.ui \
    film/MotionArea/motionbase.ui \
    film/Dialogs/cameracontroldialog.ui \
    film/MotionArea/motiontape.ui \
    film/JogControlManager/jogcontrolpanel.ui \
    film/FilmFileHandler/addressnotfounddialog.ui \
    film/Dialogs/trackinfodialog.ui \
    film/MotionArea/ErrorConsole/motionerrordialog.ui \
    film/MotionArea/ErrorConsole/motionerrorelement.ui \
    film/MotionArea/notrackswidget.ui
