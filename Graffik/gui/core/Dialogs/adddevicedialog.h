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

#ifndef ADDDEVICEDIALOG_H
#define ADDDEVICEDIALOG_H

#include <QDialog>
#include <QStringList>

#include "MoCoBus/omnetwork.h"


namespace Ui {
    class AddDeviceDialog;
}

/** Add Device Dialog Class

  This dialog allows a user to manually add a device,
  or to programmatically add one allowing the user to change a minimum amount
  of information when devices are determined automatically (e.g. via scanning).

  @author
  C. A. Church
  */
class AddDeviceDialog : public QDialog
{
    Q_OBJECT
    
public:
    AddDeviceDialog(OMNetwork* c_net, QWidget *parent = 0);
    AddDeviceDialog(OMNetwork* c_net, QString c_bus, QString c_type, unsigned short c_addr, QString c_name, QWidget *parent = 0);

    ~AddDeviceDialog();
    
    void updateBuses();

public slots:

    void on_okButton_clicked();
    void on_cancelButton_clicked();

private:
    Ui::AddDeviceDialog *ui;
    OMNetwork *_net;

    void _initInputs();

};

#endif // ADDDEVICEDIALOG_H
