
macx {
    LIBS += -framework QtHelp
}

OTHER_FILES += \
    docs/slimdoc.html \
    docs/slim-ovr.png \
    docs/docs.qhp \
    docs/docs.qhcp \
    docs/docs.qhc \
    docs/docs.qch

HEADERS += \
    docs/helpwindow.h \
    docs/helpbrowser.h

SOURCES += \
    docs/helpwindow.cpp \
    docs/helpbrowser.cpp

FORMS += \
    docs/helpwindow.ui

