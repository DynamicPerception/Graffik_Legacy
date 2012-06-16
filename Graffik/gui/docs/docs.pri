# load correct help libraries for specific OSes
# and copy correct files to correct locations during build

help_copy.target = dox
help_copy.commands = @echo "Building Help Collections";


win32 {
   CONFIG += help
}

macx {
    LIBS += -framework QtHelp
    help_copy.commands += echo "cp $$(PWD)/docs/* $$(OUT_PWD)/";
    help_copy.commands += $$[QT_INSTALL_BINS]/qhelpgenerator docs.qhp -o docs.qch;
    help_copy.commands += $$[QT_INSTALL_BINS]/qcollectiongenerator docs.qhcp -o docs.qhc;
}

QMAKE_EXTRA_TARGETS += help_copy
# POST_TARGETDEPS += dox

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

