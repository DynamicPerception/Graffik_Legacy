#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "MoCoBus/omnetwork.h"

#include "core/UserData/userdata.h"

#include "core/Dialogs/addnetdialog.h"
#include "core/Dialogs/adddevicedialog.h"

#include "core/NetworkModel/networkmodel.h"
#include "core/NetworkManager/networkmanager.h"
#include "core/AxisOptions/axisoptions.h"
#include "core/AxisOptions/axisoptionsdialog.h"

#include "Slim/slimcommandparser.h"
#include "Slim/commandhistorymodel.h"


#include "film/filmwindow.h"
#include "slim/slimwindow.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private:
    Ui::MainWindow *ui;
    SlimWindow* _slimWindow;
    FilmWindow* _filmWindow;
    AddNetDialog* _addNet;
    AddDeviceDialog* _addDev;
    OMNetwork* _net;
    NetworkModel* _netModel;
    SlimCommandParser* _parser;
    UserData* _uData;
    AxisOptions* _axisOpts;
    CommandHistoryModel* _cmdHist;

public slots:
    void on_actionOpen_File_triggered();
    void on_actionAdd_Bus_triggered();
    void on_actionAdd_Device_triggered();
    void on_actionManage_Network_triggered();
    void on_actionSave_File_triggered();
    void on_actionHelp_Contents_triggered();
    void on_actionScan_for_Devices_triggered();
    void on_actionInitialize_New_Device_triggered();

private slots:

};

#endif // MAINWINDOW_H
