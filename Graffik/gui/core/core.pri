DEFINES *= QT_USE_QSTRINGBUILDER

# un-comment to disable all debugging output from libraries

# DEFINES *= QT_NO_DEBUG_OUTPUT

HEADERS                 +=  \
			   core/Dialogs/errordialog.h \
			   core/Dialogs/addnetdialog.h \ 
    core/NetworkStatusDisplay/networkstatusdisplay.h \
    core/NetworkStatusDisplay/networkstatusgrid.h \
    core/Dialogs/adddevicedialog.h \
    core/NetworkManager/networkmanager.h \
    core/NetworkModel/networkmodel.h \
    core/UserData/userdata.h \
    core/Dialogs/confirmdialog.h \
    core/Dialogs/deletedevicedialog.h \
    core/AxisOptions/axisoptionsdialog.h \
    core/AxisOptions/axisoptions.h \
    core/Dialogs/deletebusdialog.h \ 
    core/DeviceScan/devicescandialog.h \
    core/DeviceScan/devicescanner.h \
    core/DeviceScan/deviceassigndialog.h \
    core/Dialogs/aboutdialog.h

			   
SOURCES                 +=  \
                           core/Dialogs/errordialog.cpp \
                           core/Dialogs/addnetdialog.cpp \
    core/NetworkStatusDisplay/networkstatusdisplay.cpp \
    core/NetworkStatusDisplay/networkstatusgrid.cpp \
    core/Dialogs/adddevicedialog.cpp \
    core/NetworkManager/networkmanager.cpp \
    core/NetworkModel/networkmodel.cpp \
    core/UserData/userdata.cpp \
    core/Dialogs/confirmdialog.cpp \
    core/Dialogs/deletedevicedialog.cpp \
    core/AxisOptions/axisoptionsdialog.cpp \
    core/AxisOptions/axisoptions.cpp \
    core/Dialogs/deletebusdialog.cpp \ 
    core/DeviceScan/devicescandialog.cpp \
    core/DeviceScan/devicescanner.cpp \
    core/DeviceScan/deviceassigndialog.cpp \
    core/Dialogs/aboutdialog.cpp

			   
FORMS 			+=  \
    core/NetworkStatusDisplay/networkstatusdisplay.ui \
    core/NetworkManager/networkmanager.ui \
    core/AxisOptions/axisoptionsdialog.ui \
    core/Dialogs/adddevicedialog.ui \
    core/Dialogs/confirmdialog.ui \
    core/Dialogs/deletedevicedialog.ui \
    core/Dialogs/deletebusdialog.ui \
	core/Dialogs/errordialog.ui \
   core/Dialogs/addnetdialog.ui \ 
    core/DeviceScan/devicescandialog.ui \
    core/DeviceScan/deviceassigndialog.ui \
    core/Dialogs/aboutdialog.ui
	   
	   
