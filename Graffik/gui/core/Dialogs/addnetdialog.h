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

#ifndef ADDNETDIALOG_H
#define ADDNETDIALOG_H

#include <QDialog>

#include "MoCoBus/omnetwork.h"

namespace Ui {
    class AddNetDialog;
}

/** Add Network (Bus) Dialog Class

  This dialog allows the user to select a serial port to add as a bus.

  @author
  C. A. Church
  */
class AddNetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddNetDialog(OMNetwork*, QWidget *parent = 0);
    ~AddNetDialog();

    void updateSerialPorts();

private:
    Ui::AddNetDialog *ui;
    OMNetwork *_net;
    QWidget* _parent;


public slots:
    void on_okButton_clicked();
    void on_cancelButton_clicked();
    void on_rescanButton_clicked();

private slots:
    void on_portCombo_currentIndexChanged(int);

};

#endif // ADDNETDIALOG_H
