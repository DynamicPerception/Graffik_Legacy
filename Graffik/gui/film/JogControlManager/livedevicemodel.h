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

#ifndef LIVEDEVICEMODEL_H
#define LIVEDEVICEMODEL_H

#include <QAbstractTableModel>
#include <QModelIndex>
#include <QVariant>
#include <QList>
#include <QHash>

#include "MoCoBus/omnetwork.h"


/** Live Device Model

  The Live Device Model is used by the Jog Control Panel to manage
  devices available for control via the JCM

  @author
  C. A. Church
  */

class LiveDeviceModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    LiveDeviceModel(OMNetwork* c_net, QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    int find(unsigned short p_addr);

private slots:

public slots:
    void deviceClicked(const QModelIndex & p_item);
    void deviceAdded(OMdeviceInfo* p_dev);
    void deviceDeleted(QString p_bus, unsigned short p_addr);

signals:
    void deviceSelected(unsigned short);

private:

    QList<OMdeviceInfo*> m_cacheDevs;

    QHash<unsigned short, OMdeviceInfo*> m_cacheNet;
    QList<unsigned short> m_devPos;

    QHash<unsigned short, int> m_devCacheLookup;

    OMNetwork* m_net;
};

#endif // LIVEDEVICEMODEL_H
