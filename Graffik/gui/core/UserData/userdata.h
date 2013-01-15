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

#ifndef USERDATA_H
#define USERDATA_H

#include <QSettings>
#include <QString>
#include <QHash>
#include <QVariant>


#include "MoCoBus/omnetwork.h"
#include "core/Options/axisoptions.h"
#include "core/Options/globaloptions.h"

/** User Data Class

  This class manages storing important data configured by the user to
  an ini file, persisting settings to disk as they are changed.

  A series of slots are provided which should be connected to an OMNetwork
  object, and to a GlobalOptions object to persist data as it changes.

  A series of recovery methods are also provided for recovering data persisted
  to disk back to in-memory objects.

  @author C. A. Church
  Copyright &copy; 2012 Dynamic Perception LLC

  */

class UserData : public QObject {
    Q_OBJECT
public:
    explicit UserData(QWidget* parent = 0);
    ~UserData();

    void recoverBuses(OMNetwork* p_net);
    void recoverAxisOptions(AxisOptions* p_opts);
    void recoverGlobalOptions(GlobalOptions* p_opts);

    bool exists();

    void clear();

public slots:
    void busAdded(OMbusInfo* p_bus);
    void busDeleted(QString p_port, QString p_name);
    void deviceAdded(OMbusInfo* p_bus, OMdeviceInfo* p_dev);
    void deviceOptionsChanged(OMaxisOptions* p_opts, unsigned short p_addr);
    void deviceDeleted(OMbusInfo* p_bus, unsigned short p_addr);
    void deviceOptionsRemoved(unsigned short p_addr);
    void globalOptionsChanged(GlobalOptions* p_opts);

private:
    QSettings* m_qset;
    QHash<QString, QVariant>* m_buses;
    QHash<QString, QVariant>* m_devices;
    QWidget* m_parent;

    void _recoverDevices(OMNetwork* p_net, QString p_bus, QString p_busPort);
    void _haveWritten(bool p_stat = true);

};

#endif // USERDATA_H
