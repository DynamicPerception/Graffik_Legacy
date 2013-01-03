# copy theme files into correct build/deploy locations

CONFIG(release, debug|release) {
    # un-comment to disable all debugging output from libraries

    DEFINES *= QT_NO_DEBUG_OUTPUT
}

win32 {

    CONFIG(debug, debug|release) {
        DDIR = $$OUT_PWD/debug/themes
    }
    CONFIG(release, debug|release) {
        DDIR = $$OUT_PWD/release/themes
    }
    DDIR ~= s,/,\\,g
}

macx {

    DDIR = $$OUT_PWD/Graffik.app/Contents/MacOS/themes
}

defineTest(copyThemeFiles) {
    files = $$1


    for(FILE, files) {
        FILE = $$PWD/$$FILE
        MYFILECOPY += @echo "Copying $$FILE" $$escape_expand(\\n\\t)
        # Replace slashes in paths with backslashes for Windows
        win32:FILE ~= s,/,\\,g


        THEMEFILECOPY += $$QMAKE_COPY -R $$quote($$FILE) $$quote($$DDIR) $$escape_expand(\\n\\t)
    }
    export(THEMEFILECOPY)
}

# list all theme directories which should be included as part of the base build
THEME_FILES = \
        DP-Blue


OTHER_FILES += \
       themes/README.txt \
       themes/DP-Blue/about.qss \
       themes/DP-Blue/add_device.qss \
       themes/DP-Blue/add_net.qss \
       themes/DP-Blue/axis_opts.qss \
       themes/DP-Blue/camera_opts.qss \
       themes/DP-Blue/confirm.qss \
       themes/DP-Blue/delete_bus.qss \
       themes/DP-Blue/delete_device.qss \
       themes/DP-Blue/error.qss \
       themes/DP-Blue/film.qss \
       themes/DP-Blue/global_opts.qss \
       themes/DP-Blue/img/accel-but-normal.png \
       themes/DP-Blue/img/accel-but-pressed.png \
       themes/DP-Blue/img/affirmative-big-button-normal.png \
       themes/DP-Blue/img/affirmative-big-button-pressed.png \
       themes/DP-Blue/img/axis-dist-bg.png \
       themes/DP-Blue/img/black-but-normal.png \
       themes/DP-Blue/img/black-but-pressed.png \
       themes/DP-Blue/img/blue-but-normal.png \
       themes/DP-Blue/img/camset-but-desat.png \
       themes/DP-Blue/img/camset-but-normal.png \
       themes/DP-Blue/img/camset-but-pressed.png \
       themes/DP-Blue/img/dark-toggle-off.png \
       themes/DP-Blue/img/dark-toggle-on.png \
       themes/DP-Blue/img/devices-but-normal.png \
       themes/DP-Blue/img/devices-but-pressed.png \
       themes/DP-Blue/img/dial-back.png \
       themes/DP-Blue/img/dial-pointer.png \
       themes/DP-Blue/img/drop-down-sm-black.png \
       themes/DP-Blue/img/drop-down-sm-blue.png \
       themes/DP-Blue/img/edit-icon-sm.png \
       themes/DP-Blue/img/fwd-end-normal.png \
       themes/DP-Blue/img/fwd-end-pressed.png \
       themes/DP-Blue/img/fwd-normal.png \
       themes/DP-Blue/img/fwd-pressed.png \
       themes/DP-Blue/img/kinetic-bg.png \
       themes/DP-Blue/img/kinetic-sm-bg.png \
       themes/DP-Blue/img/kinetic-sm-wheel.png \
       themes/DP-Blue/img/kinetic-wheel.png \
       themes/DP-Blue/img/load-but-normal.png \
       themes/DP-Blue/img/load-but-pressed.png \
       themes/DP-Blue/img/mute-but-normal.png \
       themes/DP-Blue/img/mute-but-pressed.png \
       themes/DP-Blue/img/pause-normal.png \
       themes/DP-Blue/img/pause-pressed.png \
       themes/DP-Blue/img/play-normal.png \
       themes/DP-Blue/img/play-pressed.png \
       themes/DP-Blue/img/res-but-normal.png \
       themes/DP-Blue/img/res-but-pressedl.png \
       themes/DP-Blue/img/rwd-home-normal.png \
       themes/DP-Blue/img/rwd-home-pressed.png \
       themes/DP-Blue/img/rwd-normal.png \
       themes/DP-Blue/img/rwd-pressed.png \
       themes/DP-Blue/img/save-but-normal.png \
       themes/DP-Blue/img/save-but-pressed.png \
       themes/DP-Blue/img/scale-but-normal.png \
       themes/DP-Blue/img/scale-but-pressed.png \
       themes/DP-Blue/img/scroll-spin-bg-dis.png \
       themes/DP-Blue/img/scroll-spin-bg.png \
       themes/DP-Blue/img/scroll-spin-down.png \
       themes/DP-Blue/img/scroll-spin-up.png \
       themes/DP-Blue/img/sel-list-bg.png \
       themes/DP-Blue/img/set-but-normal.png \
       themes/DP-Blue/img/set-but-pressed.png \
       themes/DP-Blue/img/set-end-normal.png \
       themes/DP-Blue/img/set-end-sel.png \
       themes/DP-Blue/img/set-home-normal.png \
       themes/DP-Blue/img/set-home-sel.png \
       themes/DP-Blue/img/slider-back-gray.png \
       themes/DP-Blue/img/slider-back.png \
       themes/DP-Blue/img/slider-handle-blue.png \
       themes/DP-Blue/img/slider-handle-dark.png \
       themes/DP-Blue/img/slider-handle.png \
       themes/DP-Blue/img/small-blue-but-normal.png \
       themes/DP-Blue/img/small-gray-but-normal.png \
       themes/DP-Blue/img/stat-led-blue.png \
       themes/DP-Blue/img/stat-led-green.png \
       themes/DP-Blue/img/stat-led-grey.png \
       themes/DP-Blue/img/stat-led-red.png \
       themes/DP-Blue/img/stop-normal.png \
       themes/DP-Blue/img/stop-pressed.png \
       themes/DP-Blue/img/toggle-bg-off.png \
       themes/DP-Blue/img/toggle-bg-on.png \
       themes/DP-Blue/img/toggle-bg.png \
       themes/DP-Blue/img/track-back-error.png \
       themes/DP-Blue/img/track-back-mute.png \
       themes/DP-Blue/img/track-back.png \
       themes/DP-Blue/img/track-div-back.png \
       themes/DP-Blue/img/Untitled-25.png \
       themes/DP-Blue/jog.qss \
       themes/DP-Blue/main.qss \
       themes/DP-Blue/motionbase.qss \
       themes/DP-Blue/net_base.qss \
       themes/DP-Blue/net_man.qss \
       themes/DP-Blue/slim.qss \
       themes/DP-Blue/theme.ini

copyThemeFiles($$THEME_FILES)

theme_copy.target = themes
theme_copy.commands = @echo "Copying Theme files to $$DDIR"  $$escape_expand(\\n\\t)

win32 {

    # exists does not like backslashes
  EDIR = $$DDIR
  EDIR ~= s,\\\\,/,g

  exists($$DDIR) {
    theme_copy.commands += @echo "Removing Directory $$EDIR" $$escape_expand(\\n\\t)
    theme_copy.commands += rd /S /Q $$DDIR $$escape_expand(\\n\\t)
  }

  theme_copy.commands += @echo "Creating Theme Directory: $$DDIR ($$EDIR)" $$escape_expand(\\n\\t)
  theme_copy.commands += md $$DDIR $$escape_expand(\\n\\t)
  theme_copy.commands += $$THEMEFILECOPY

}

macx {

 !exists($$DDIR) {
    theme_copy.commands += @echo "Creating Theme Directory: $$DDIR" $$escape_expand(\\n\\t)
    theme_copy.commands += mkdir -p $$DDIR $$escape_expand(\\n\\t)
  }

  theme_copy.commands += $$THEMEFILECOPY

}

QMAKE_EXTRA_TARGETS += theme_copy
POST_TARGETDEPS += themes
