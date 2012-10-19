/*

  Graffik Motion Control Application

  Copyright (c) 2011-2012 Dynamic Perception

 This file is part of Graffik.

    Graffik is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Graffik is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Graffik.  If not, see <http://www.gnu.org/licenses/>.

    */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "MoCoBus/omnetwork.h"

#include "core/UserData/userdata.h"

#include "core/Dialogs/addnetdialog.h"
#include "core/Dialogs/adddevicedialog.h"
#include "core/Dialogs/aboutdialog.h"
#include "core/Dialogs/netbasedialog.h"


#include "core/NetworkModel/networkmodel.h"
#include "core/NetworkManager/networkmanager.h"

#include "core/Options/axisoptions.h"
#include "core/Options/axisoptionsdialog.h"
#include "core/Options/globaloptions.h"
#include "core/Options/globaloptionsdialog.h"

#include "Slim/slimcommandparser.h"

#include "slim/CommandHistoryModel/commandhistorymodel.h"


#include "film/filmwindow.h"
#include "slim/slimwindow.h"

namespace Ui {
    class MainWindow;
}

/** Main Window Class

  This is the top-level main window for the application.  Presents a common
  header area, and loads in panels for the main UI sections.  Controls panel
  display.

  @author
  C. A. Church
  */

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
    GlobalOptions* _globalOpts;

public slots:
    void on_actionHelp_Contents_triggered();
    void on_actionSettings_triggered();
    void on_actionAbout_Graffik_triggered();

    void on_filmButton_clicked();
    void on_scriptButton_clicked();
    void on_netButton_clicked();
    void on_optButton_clicked();

private slots:

    void globalOptionsChanged();

signals:
    void globalOptionsChanged(GlobalOptions* p_opts);

};

#endif // MAINWINDOW_H
