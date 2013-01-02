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

#ifndef DELETEBUSDIALOG_H
#define DELETEBUSDIALOG_H

#include <QDialog>
#include <QList>
#include <QString>

#include "MoCoBus/omnetwork.h"
#include "core/Themer/singlethemer.h"

namespace Ui {
class DeleteBusDialog;
}

/** Delete Bus Dialog Class

  This dialog allows the user to select a bus for deletion

  @author
  C. A. Church
  */
class DeleteBusDialog : public QDialog
{
    Q_OBJECT
    
public:
    DeleteBusDialog(OMNetwork* c_net, QWidget *parent = 0);
    ~DeleteBusDialog();
    
private slots:
    void on_okButton_clicked();
    void on_cancelButton_clicked();

private:
    Ui::DeleteBusDialog *ui;
    OMNetwork* m_net;
    QList<QString> m_buses;
};

#endif // DELETEBUSDIALOG_H
