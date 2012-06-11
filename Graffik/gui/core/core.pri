DEFINES *= QT_USE_QSTRINGBUILDER


HEADERS                 += core/SpeedDial/dialspeed.h \
			   core/ErrorDialog/errordialog.h \
			   core/AddNetDialog/addnetdialog.h \ 
    core/NetworkStatusDisplay/networkstatusdisplay.h \
    core/NetworkStatusDisplay/networkstatusgrid.h \
    core/AddDeviceDialog/adddevicedialog.h \
    core/motionarea.h \
    core/NetworkManager/networkmanager.h \
    core/NetworkModel/networkmodel.h \
 #   core/AxisTimeline/axistimeline.h
    core/UserData/userdata.h \
    core/ConfirmDialog/confirmdialog.h \
    core/DeleteDeviceDialog/deletedevicedialog.h
			   
SOURCES                 += core/SpeedDial/dialspeed.cpp \
                           core/ErrorDialog/errordialog.cpp \
                           core/AddNetDialog/addnetdialog.cpp \
    core/NetworkStatusDisplay/networkstatusdisplay.cpp \
    core/NetworkStatusDisplay/networkstatusgrid.cpp \
    core/AddDeviceDialog/adddevicedialog.cpp \
    core/motionarea.cpp \
    core/NetworkManager/networkmanager.cpp \
    core/NetworkModel/networkmodel.cpp \
 #   core/AxisTimeline/axistimeline.cpp
    core/UserData/userdata.cpp \
    core/ConfirmDialog/confirmdialog.cpp \
    core/DeleteDeviceDialog/deletedevicedialog.cpp
			   
FORMS 			+= core/SpeedDial/dialspeed.ui \
			   core/ErrorDialog/errordialog.ui \
			   core/AddNetDialog/addnetdialog.ui \
    core/NetworkStatusDisplay/networkstatusdisplay.ui \
    core/AddDeviceDialog/adddevicedialog.ui \
    core/motionarea.ui \
    core/NetworkManager/networkmanager.ui \
  #  core/AxisTimeline/axistimeline.ui
    core/ConfirmDialog/confirmdialog.ui \
    core/DeleteDeviceDialog/deletedevicedialog.ui
	   
	   
