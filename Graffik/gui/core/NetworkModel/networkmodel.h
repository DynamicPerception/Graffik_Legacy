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

#ifndef NETWORKMODEL_H
#define NETWORKMODEL_H

#include <QStandardItemModel>

#include "MoCoBus/omnetwork.h"

  // which column holds the configure icon?
#define NETMODEL_CONFIG_COL 3
#define NETMODEL_ADDR_COL 1

/** Network Model Class

  This class creates a model for display of network elements.

  @author
  C. A. Church
  */
class NetworkModel : public QStandardItemModel
{
    Q_OBJECT
public:
    NetworkModel(OMNetwork* c_net, QObject *parent = 0);
    ~NetworkModel();

    OMNetwork* net();

signals:
    
public slots:

    void addBus(QString);
    void busDeleted(QString, QString);
    void addDevice(QString, unsigned short);
    void deviceDeleted(QString, unsigned short);

private:
    OMNetwork* m_net;

    void _setHeaders();
};

#endif // NETWORKMODEL_H
