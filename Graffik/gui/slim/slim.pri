DEFINES *= QT_USE_QSTRINGBUILDER

CONFIG(release, debug|release) {
    # un-comment to disable all debugging output from libraries

    DEFINES *= QT_NO_DEBUG_OUTPUT
}

HEADERS += \
    slim/slimwindow.h \
    slim/SlimFileHandler/slimfilehandler.h \
    slim/CommandHistoryModel/commandhistorymodel.h \
    slim/CommandHistoryView/commandhistoryview.h

SOURCES += \
    slim/slimwindow.cpp \
    slim/SlimFileHandler/slimfilehandler.cpp \
    slim/CommandHistoryModel/commandhistorymodel.cpp \
    slim/CommandHistoryView/commandhistoryview.cpp

FORMS += \
    slim/slimwindow.ui


