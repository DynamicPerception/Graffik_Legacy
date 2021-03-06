# load correct help libraries for specific OSes
# and copy correct files to correct locations during build

CONFIG(release, debug|release) {
    # un-comment to disable all debugging output from libraries

    DEFINES *= QT_NO_DEBUG_OUTPUT
}

win32 {

    CONFIG += help

    CONFIG(debug, debug|release) {
        DDIR = $$OUT_PWD/debug/docs
    }
    CONFIG(release, debug|release) {
        DDIR = $$OUT_PWD/release/docs
    }
    DDIR ~= s,/,\\,g
}

macx {
    LIBS += -framework QtHelp

    DDIR = $$OUT_PWD/Graffik.app/Contents/MacOS/docs
}

defineTest(copyHelpFiles) {
    files = $$1


    for(FILE, files) {
        FILE ~= s,docs/,,
        FILE = $$PWD/$$FILE
        MYFILECOPY += @echo "Copying $$FILE" $$escape_expand(\\n\\t)
        # Replace slashes in paths with backslashes for Windows
        win32:FILE ~= s,/,\\,g

        macx {
            MYFILECOPY += $$QMAKE_COPY $$quote($$FILE) $$quote($$DDIR) $$escape_expand(\\n\\t)
        }
        win32 {
            MYFILECOPY += $$QMAKE_COPY /Y $$quote($$FILE) $$quote($$DDIR) $$escape_expand(\\n\\t)
        }
    }
    export(MYFILECOPY)
}


OTHER_FILES += \
    docs/slimdoc.html \
    docs/slim-ovr.png \
    docs/chart_miqpos.png \
    docs/chart_miqspeed.png \
    docs/chart_mposprofile.png \
    docs/chart_mqs.png \
    docs/chart_mspeedprofile.png \
    docs/slim-mn.png \
    docs/slim-scan1.png \
    docs/slim-scan2.png \
    docs/slim-scan3.png \
    docs/GraffikIntro.html \
    docs/film-ovr.png \
    docs/docs.qhp \
    docs/docs.qhcp

HEADERS += \
    docs/helpwindow.h \
    docs/helpbrowser.h

SOURCES += \
    docs/helpwindow.cpp \
    docs/helpbrowser.cpp

FORMS += \
    docs/helpwindow.ui




copyHelpFiles($$OTHER_FILES)

help_copy.target = dox
help_copy.commands = @echo "Building Help Files in $$DDIR"  $$escape_expand(\\n\\t)

win32 {

    # exists does not like backslashes
  EDIR = $$DDIR
  EDIR ~= s,\\\\,/,g


  exists($$DDIR) {
    help_copy.commands += @echo "Docs Dir Exists, Removing Docs Directory: $$DDIR" $$escape_expand(\\n\\t)
    help_copy.commands += rd /S /Q $$DDIR $$escape_expand(\\n\\t)
  }

  help_copy.commands += @echo "Creating Docs Directory: $$DDIR ($$EDIR)" $$escape_expand(\\n\\t)
  help_copy.commands += md $$DDIR $$escape_expand(\\n\\t)
  help_copy.commands += $$MYFILECOPY
  help_copy.commands += @echo "Running qhelpgenerator" $$escape_expand(\\n\\t)
  help_copy.commands += $$[QT_INSTALL_BINS]\\qhelpgenerator $$DDIR\\docs.qhp -o $$DDIR\\docs.qch $$escape_expand(\\n\\t)
  help_copy.commands += @echo "Running qcollectiongenerator" $$escape_expand(\\n\\t)
  help_copy.commands += $$[QT_INSTALL_BINS]\\qcollectiongenerator $$DDIR\\docs.qhcp -o $$DDIR\\docs.qhc $$escape_expand(\\n\\t)
}

macx {

 !exists($$DDIR) {
    help_copy.commands += @echo "Creating Docs Directory: $$DDIR" $$escape_expand(\\n\\t)
    help_copy.commands += mkdir -p $$DDIR $$escape_expand(\\n\\t)
  }

  help_copy.commands += $$MYFILECOPY
  help_copy.commands += @echo "Running qhelpgenerator" $$escape_expand(\\n\\t)
  help_copy.commands += $$[QT_INSTALL_BINS]/qhelpgenerator $$DDIR/docs.qhp -o $$DDIR/docs.qch $$escape_expand(\\n\\t)
  help_copy.commands += @echo "Running qcollectiongenerator" $$escape_expand(\\n\\t)
  help_copy.commands += $$[QT_INSTALL_BINS]/qcollectiongenerator $$DDIR/docs.qhcp -o $$DDIR/docs.qhc $$escape_expand(\\n\\t)
}

QMAKE_EXTRA_TARGETS += help_copy
POST_TARGETDEPS += dox
