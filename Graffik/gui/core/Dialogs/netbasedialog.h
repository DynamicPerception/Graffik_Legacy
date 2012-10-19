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

#ifndef NETBASEDIALOG_H
#define NETBASEDIALOG_H

#include <QDialog>

#include "MoCoBus/omnetwork.h"

#include "core/DeviceScan/devicescanner.h"
#include "core/Options/axisoptions.h"

#include "core/NetworkManager/networkmanager.h"
#include "core/NetworkModel/networkmodel.h"

namespace Ui {
class NetBaseDialog;
}

/** Net Base Class

  This dialog displays basic device operations.

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

private:
    Ui::NetBaseDialog *ui;
    OMNetwork* m_net;

    DeviceScanner* m_scan;
    AxisOptions* m_opts;
    NetworkModel* m_model;
};

#endif // NETBASEDIALOG_H
