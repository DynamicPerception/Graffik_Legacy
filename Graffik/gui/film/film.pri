DEFINES += QT_USE_QSTRINGBUILDER
	   

HEADERS += \
    film/filmwindow.h \
    film/LiveDeviceModel/livedevicemodel.h \
    film/JogControlManager/speedcontrolproxy.h \
    film/FilmGlobalOptions/filmglobaloptions.h \
    film/JogControlManager/jogcontrolmanager.h \
    film/FilmParameters/filmparameters.h \
    film/MotionArea/motionarea.h \
    film/MotionArea/motionbase.h \
    film/Dialogs/cameracontroldialog.h


SOURCES += \
    film/filmwindow.cpp \
    film/LiveDeviceModel/livedevicemodel.cpp \
    film/JogControlManager/speedcontrolproxy.cpp \
    film/FilmGlobalOptions/filmglobaloptions.cpp \
    film/JogControlManager/jogcontrolmanager.cpp \
    film/FilmParameters/filmparameters.cpp \
    film/MotionArea/motionarea.cpp \
    film/MotionArea/motionbase.cpp \
    film/Dialogs/cameracontroldialog.cpp

FORMS += \
    film/filmwindow.ui \
    film/MotionArea/motionarea.ui \
    film/MotionArea/motionbase.ui \
    film/Dialogs/cameracontroldialog.ui
