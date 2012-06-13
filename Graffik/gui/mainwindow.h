#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "MoCoBus/omnetwork.h"

#include "core/UserData/userdata.h"

#include "core/AddNetDialog/addnetdialog.h"
#include "core/AddDeviceDialog/adddevicedialog.h"

#include "core/NetworkModel/networkmodel.h"
#include "core/NetworkManager/networkmanager.h"
#include "core/AxisOptions/axisoptions.h"
#include "core/AxisOptions/axisoptionsdialog.h"

#include "Slim/slimcommandparser.h"
#include "Slim/commandhistorymodel.h"


#include "slim/slimwindow.h"
#include "film/filmwindow.h"

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
    filmWindow* _filmWindow;
    addNetDialog* _addNet;
    addDeviceDialog* _addDev;
    OMNetwork* _net;
    networkModel* _netModel;
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

private slots:

};

#endif // MAINWINDOW_H
