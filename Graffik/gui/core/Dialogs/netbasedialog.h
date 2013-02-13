/*

  Graffik Motion Control Application

  Copyright (c) 2011-2013 Dynamic Perception

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

#ifndef NETBASEDIALOG_H
#define NETBASEDIALOG_H

#include <QDialog>
#include <QVBoxLayout>

#include "MoCoBus/omnetwork.h"

#include "core/DeviceScan/devicescanner.h"
#include "core/Options/axisoptions.h"

#include "core/NetworkManager/networkmanager.h"
#include "core/NetworkModel/networkmodel.h"

#include "core/Themer/singlethemer.h"

namespace Ui {
class NetBaseDialog;
}

/** Net Base Class

  This dialog displays basic device operations, such as scanning, managing, initializing, etc.

  This is a dialog that is expected to have one or more sub-widgets displayed within a frame contained
  in the dialog.

  @author
  C. A. Church
  */

class NetBaseDialog : public QDialog
{
    Q_OBJECT
    
public:
    NetBaseDialog(OMNetwork* c_net, NetworkModel* c_model, AxisOptions* c_opts, QWidget *parent = 0);
    ~NetBaseDialog();
    
public slots:
    void on_scanButton_clicked();
    void on_initButton_clicked();
    void on_manageButton_clicked();
    void on_doneButton_clicked();

    void scanCompleted();

private slots:


private:
    Ui::NetBaseDialog *ui;
    OMNetwork* m_net;

    DeviceScanner* m_scan;
    AxisOptions* m_opts;
    NetworkModel* m_model;
    NetworkManager* m_netMan;

    QWidget* m_curWidget;

    QVBoxLayout* m_frameLayout;

    void _swapWidget(QWidget* p_widg);
    void _blockForScan(bool p_stat);

    enum curState { SCAN, INIT, MANAGE };

    curState m_curState;

};

#endif // NETBASEDIALOG_H
