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

#ifndef DELETEDEVICEDIALOG_H
#define DELETEDEVICEDIALOG_H

#include <QDialog>
#include <QList>
#include <QHash>

#include "MoCoBus/omnetwork.h"

namespace Ui {
class DeleteDeviceDialog;
}

/** Delete Device Dialog Class

  This dialog allows the user to select a device for deletion

  @author
  C. A. Church
  */
class DeleteDeviceDialog : public QDialog
{
    Q_OBJECT
    
public:
    DeleteDeviceDialog(OMNetwork *c_net, QWidget *parent = 0);
    ~DeleteDeviceDialog();
    
private slots:
    void on_okButton_clicked();
    void on_cancelButton_clicked();

private:
    Ui::DeleteDeviceDialog *ui;
    OMNetwork* m_net;
    QList< QHash<unsigned short, QString>* > m_devList;
};

#endif // DELETEDEVICEDIALOG_H
