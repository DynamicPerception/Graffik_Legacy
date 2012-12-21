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

#ifndef DEVICEASSIGNDIALOG_H
#define DEVICEASSIGNDIALOG_H

#include <QDialog>
#include <QString>

#include "MoCoBus/omnetwork.h"
#include "Devices/nanoMoCo/omaxis.h"

namespace Ui {
class DeviceAssignDialog;
}


/** Device Assign Dialog Class

  This dialog is shown when a new scanned device is found, and allows the
  user to define the new address and name for the device.

  @author
  C. A. Church
  */

class DeviceAssignDialog : public QDialog
{
    Q_OBJECT
    
public:
    DeviceAssignDialog(OMNetwork* c_net, QString c_bus, QString c_type, QString c_name, QWidget *parent = 0);
    ~DeviceAssignDialog();
    
public slots:
    void on_saveButton_clicked();
    void on_cancelButton_clicked();

private slots:
    void _commandComplete(int p_id, OMCommandBuffer* p_cmd);

private:
    Ui::DeviceAssignDialog *ui;

    OMNetwork* m_net;
    QString m_bus;
    QString m_type;
    QString m_name;
    int m_cmdId;
    QString m_newName;
    unsigned short m_newAddr;

    QHash<int, unsigned short> m_cmdSent;

    void _changeAddr(unsigned short p_old, unsigned short p_new);
    void _initInputs(QString p_type);
    void _postInit(QString p_bus, unsigned short p_addr, QString p_type, QString p_name);
};

#endif // DEVICEASSIGNDIALOG_H
