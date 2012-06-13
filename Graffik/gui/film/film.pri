DEFINES *= QT_USE_QSTRINGBUILDER
	   

HEADERS += \
    film/filmwindow.h \
    film/LiveDeviceModel/livedevicemodel.h \
    film/JogControlManager/speedcontrolproxy.h \
    film/FilmGlobalOptions/filmglobaloptions.h \
    film/JogControlManager/jogcontrolmanager.h \
    film/FilmParameters/filmparameters.h \

SOURCES += \
    film/filmwindow.cpp \
    film/LiveDeviceModel/livedevicemodel.cpp \
    film/JogControlManager/speedcontrolproxy.cpp \
    film/FilmGlobalOptions/filmglobaloptions.cpp \
    film/JogControlManager/jogcontrolmanager.cpp \
    film/FilmParameters/filmparameters.cpp \

FORMS += \
    film/filmwindow.ui \
