DEFINES *= QT_USE_QSTRINGBUILDER

CONFIG(release, debug|release) {
    # un-comment to disable all debugging output from libraries

    DEFINES *= QT_NO_DEBUG_OUTPUT
}

HEADERS                 +=  \
			   core/Dialogs/errordialog.h \
			   core/Dialogs/addnetdialog.h \ 
    core/Dialogs/adddevicedialog.h \
    core/NetworkManager/networkmanager.h \
    core/NetworkModel/networkmodel.h \
    core/UserData/userdata.h \
    core/Dialogs/confirmdialog.h \
    core/Dialogs/deletedevicedialog.h \
    core/Options/axisoptionsdialog.h \
    core/Options/axisoptions.h \
    core/Dialogs/deletebusdialog.h \ 
    core/DeviceScan/devicescanwidget.h \
    core/DeviceScan/devicescanner.h \
    core/DeviceScan/deviceassigndialog.h \
    core/Dialogs/aboutdialog.h \
    core/Options/globaloptions.h \
    core/Options/globaloptionsdialog.h \
    core/Options/optiontypes.h \
    core/Utilities/timeconverter.h \
    core/Dialogs/netbasedialog.h \
    core/Utilities/flickcharm.h \
    core/Utilities/flicknumber.h \
    core/Widgets/slidetoggle.h \
    core/Widgets/skinneddial.h \
    core/Utilities/flicknumberitem.h \
    core/Widgets/statefulbutton.h \
    core/Themer/themer.h \
    core/Utilities/singleton.h \
    core/Themer/singlethemer.h \
    core/Dialogs/welcomedialog.h \
    core/Utilities/basiceventfilter.h \
    core/Widgets/statefullabel.h

			   
SOURCES                 +=  \
                           core/Dialogs/errordialog.cpp \
                           core/Dialogs/addnetdialog.cpp \
    core/Dialogs/adddevicedialog.cpp \
    core/NetworkManager/networkmanager.cpp \
    core/NetworkModel/networkmodel.cpp \
    core/UserData/userdata.cpp \
    core/Dialogs/confirmdialog.cpp \
    core/Dialogs/deletedevicedialog.cpp \
    core/Options/axisoptionsdialog.cpp \
    core/Options/axisoptions.cpp \
    core/Dialogs/deletebusdialog.cpp \ 
    core/DeviceScan/devicescanwidget.cpp \
    core/DeviceScan/devicescanner.cpp \
    core/DeviceScan/deviceassigndialog.cpp \
    core/Dialogs/aboutdialog.cpp \
    core/Options/globaloptions.cpp \
    core/Options/globaloptionsdialog.cpp \
    core/Utilities/timeconverter.cpp \
    core/Dialogs/netbasedialog.cpp \
    core/Utilities/flickcharm.cpp \
    core/Utilities/flicknumber.cpp \
    core/Widgets/slidetoggle.cpp \
    core/Widgets/skinneddial.cpp \
    core/Utilities/flicknumberitem.cpp \
    core/Widgets/statefulbutton.cpp \
    core/Themer/themer.cpp \
    core/Themer/singlethemer.cpp \
    core/Dialogs/welcomedialog.cpp \
    core/Utilities/basiceventfilter.cpp \
    core/Widgets/statefullabel.cpp

			   
FORMS 			+=  \
    core/NetworkManager/networkmanager.ui \
    core/Options/axisoptionsdialog.ui \
    core/Dialogs/adddevicedialog.ui \
    core/Dialogs/confirmdialog.ui \
    core/Dialogs/deletedevicedialog.ui \
    core/Dialogs/deletebusdialog.ui \
	core/Dialogs/errordialog.ui \
   core/Dialogs/addnetdialog.ui \ 
    core/DeviceScan/devicescanwidget.ui \
    core/DeviceScan/deviceassigndialog.ui \
    core/Dialogs/aboutdialog.ui \
    core/Options/globaloptionsdialog.ui \
    core/Dialogs/netbasedialog.ui \
    core/Utilities/flicknumber.ui \
    core/Utilities/flicknumberitem.ui \
    core/Dialogs/welcomedialog.ui
	   

OTHER_FILES +=
