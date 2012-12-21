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

#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QWidget>
#include <QPixmap>

#include "MoCoBus/omnetwork.h"


#include "core/NetworkModel/networkmodel.h"
#include "core/Dialogs/addnetdialog.h"
#include "core/Dialogs/adddevicedialog.h"
#include "core/Dialogs/deletedevicedialog.h"
#include "core/Dialogs/deletebusdialog.h"
#include "core/Options/axisoptions.h"

namespace Ui {
class NetworkManager;
}

/** Network Manager Dialog Class

  This dialog displays information from the NetworkModel, along with buttons
  to manage the network.

  @author
  C. A. Church
  */

class NetworkManager : public QWidget
{
    Q_OBJECT
    /** Icon Image Property

      Sets the icon image for the edit icon for each device displayed
      */
    Q_PROPERTY(QPixmap editIcon READ editIcon WRITE setEditIcon DESIGNABLE true)

public:
    NetworkManager(NetworkModel* c_net, AxisOptions* c_opts, QWidget *parent = false);
    ~NetworkManager();
    
    QPixmap editIcon();
    void setEditIcon(QPixmap p_img);

private:
    Ui::NetworkManager *ui;
    NetworkModel* _netMod;
    AxisOptions* _opts;

private slots:
    void on_addBusButton_clicked();
    void on_addDevButton_clicked();
    void on_delDevButton_clicked();
    void on_delBusButton_clicked();
    void onRowClick(QModelIndex);
};

#endif // NETWORKMANAGER_H
