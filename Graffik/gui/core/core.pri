DEFINES += QT_USE_QSTRINGBUILDER


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
    core/Dialogs/deletebusdialog.h 

			   
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
    core/Dialogs/deletebusdialog.cpp 

			   
FORMS 			+=  \
    core/NetworkStatusDisplay/networkstatusdisplay.ui \
    core/NetworkManager/networkmanager.ui \
    core/AxisOptions/axisoptionsdialog.ui \
    core/Dialogs/adddevicedialog.ui \
    core/Dialogs/confirmdialog.ui \
    core/Dialogs/deletedevicedialog.ui \
    core/Dialogs/deletebusdialog.ui \
	core/Dialogs/errordialog.ui \
   core/Dialogs/addnetdialog.ui 
	   
	   
