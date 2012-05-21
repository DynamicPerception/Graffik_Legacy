#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "MoCoBus/omnetwork.h"

#include "core/UserData/userdata.h"

#include "core/AddNetDialog/addnetdialog.h"
#include "core/AddDeviceDialog/adddevicedialog.h"

#include "core/NetworkModel/networkmodel.h"
#include "core/NetworkManager/networkmanager.h"

    // TODO: this stuff needs to either move to Film
    // or be changed to core

#include "film/OMAxisFilmOptions/omaxisfilmoptions.h"
#include "film/OMAxisFilmOptions/omaxisfilmoptionsdialog.h"

#include "Slim/slimcommandparser.h"


#include "slimwindow.h"
#include "filmwindow.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

   // void setSlimWindow(SlimWindow*);

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
    OMAxisFilmOptions* _axisOpts;

public slots:
    void on_actionOpen_File_triggered();
    void on_actionAdd_Bus_triggered();
    void on_actionAdd_Device_triggered();
    void on_actionManage_Network_triggered();

private slots:
   // void on_slimButton_clicked();

};

#endif // MAINWINDOW_H
