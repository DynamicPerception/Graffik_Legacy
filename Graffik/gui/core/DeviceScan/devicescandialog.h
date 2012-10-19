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

#ifndef DEVICESCANDIALOG_H
#define DEVICESCANDIALOG_H

#include <QDialog>
#include <QString>


namespace Ui {
class DeviceScanDialog;
}


/** Device Scan Dialog Class

  This dialog is shown when scanning for new devices, to be controlled
  by the DeviceScanner

  @author
  C. A. Church
  */

class DeviceScanDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit DeviceScanDialog(QWidget *parent = 0);
    ~DeviceScanDialog();

    void enableConfirm(bool p_enable);
    void addNote(QString p_note);
    void totalNodes(int p_nodes);
    void scannedNodes(int p_nodes);
    void nodesFound(bool p_found);

public slots:
    void on_doneButton_clicked();
    void on_cancelButton_clicked();
        
private:
    Ui::DeviceScanDialog *ui;
};

#endif // DEVICESCANDIALOG_H
