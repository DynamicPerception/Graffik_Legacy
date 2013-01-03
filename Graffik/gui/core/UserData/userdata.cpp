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

#include "userdata.h"

#include <QCoreApplication>
#include <QDebug>

#include "core/Dialogs/errordialog.h"

/** Constructor

  Create a new UserData object.

  */

UserData::UserData(QWidget* parent) : QObject()
{
    m_parent = parent;

    QCoreApplication::setOrganizationName("Dynamic Perception");
    QCoreApplication::setOrganizationDomain("dynamicperception.com");
    QCoreApplication::setApplicationName("Graffik");

    m_qset = new QSettings(QSettings::IniFormat, QSettings::UserScope, "Dynamic Perception", "Graffik");

}

UserData::~UserData() {
    delete m_qset;
}

/** Settings Exist

  Returns whether or not the settings file exists.

  If settings file does not already exist, returns false.

  Is best used to determine if the current installation has ever
  been configured.

  */

bool UserData::exists() {
    return m_qset->contains("stat");
}

void UserData::busAdded(OMbusInfo *p_bus) {

     qDebug() << "UD:BusAdded: Got bus" << p_bus->name;

    QString root_key = "network/buses/" + p_bus->name + "/";
    QString p_key = root_key + "port";

    m_qset->setValue(p_key, p_bus->bus->port());


}

void UserData::deviceAdded(OMbusInfo *p_bus, OMdeviceInfo *p_dev) {

    qDebug() << "UD:deviceAdded: Got Device" << p_dev->name;
    QString root_key = "network/buses/" + p_bus->name + "/devices/" + QString::number(p_dev->device->address());
    m_qset->beginGroup(root_key);

    m_qset->setValue("name", p_dev->name);
    m_qset->setValue("type", p_dev->type);
    m_qset->endGroup();

}

void UserData::deviceOptionsChanged(OMaxisOptions *p_opts, unsigned short p_addr) {

    qDebug() << "UD:Got request to write axis options for axis" << p_addr;

    QString key = "film/device_options/" + QString::number(p_addr);

        // this really shouldn't happen... but just in case, let's not try to use an invalid pointer
        // (some spurious agent may get an invalid device option, and then try to save it back)
    if( p_opts == 0 )
        return;

    m_qset->setValue(key, QVariant::fromValue(*p_opts));
}

void UserData::globalOptionsChanged(GlobalOptions *p_opts) {

    qDebug() << "UD: Got Global Opts Change";

    m_qset->beginGroup("global/options");

    m_qset->setValue("stop", p_opts->stopOnErr());
    m_qset->setValue("display", p_opts->display());
    m_qset->setValue("theme", p_opts->theme());

    m_qset->endGroup();
}

/** Recover Global Options

  Recovers any stored Global Options to the supplied GlobalOptions object

  */

void UserData::recoverGlobalOptions(GlobalOptions *p_opts) {
    qDebug() << "UD: recoverGlobalOptions";

    m_qset->beginGroup("global/options");

    bool     stop = m_qset->value("stop").toBool();
    int      disp = m_qset->value("display").toInt();
    QString theme = m_qset->value("theme").toString();

    p_opts->stopOnErr(stop);
    p_opts->display(disp);
    p_opts->theme(theme);

    m_qset->endGroup();
}

/** Recover Axis Options

  Recovers any stored axis options to the supplied AxisOptions object

  */

void UserData::recoverAxisOptions(AxisOptions *p_opts) {

    qDebug() << "UD: recoverAxisOptions: Started";

    m_qset->beginGroup("film/device_options/");

    QStringList devices = m_qset->childKeys();
    QString device;

    foreach(device, devices) {
        qDebug() << "UD: recoverAxisOptions: recovering device" << device;
        OMaxisOptions thsopts = m_qset->value(device).value< OMaxisOptions >();
        OMaxisOptions* newopts = new OMaxisOptions(thsopts);

        p_opts->setOptions(device.toUShort(), newopts);
    }


    m_qset->endGroup();
}

/** Recover Saved Buses

  This method recovers any buses saved to the ini file, re-creating them
  in the supplied OMNetwork object, and recovers any devices associated with
  them.

  This method will display an error dialog to the user if the buses cannot be
  recovered for any reason.

  If any bus can be recovered, the devices associated with that bus will be
  recovered as well.

  */

void UserData::recoverBuses(OMNetwork *p_net) {
    m_qset->beginGroup("network/buses");

    QStringList buses = m_qset->childGroups();
    QString bus;

        // re-create each bus, and each of its devices
    foreach(bus, buses) {

        bool ok = true;

        qDebug() << "UD:recoverBuses: Found bus" << bus;


        QString root_key = bus + "/";
        QString bus_port = m_qset->value(root_key + "port").toString();

        try {
            p_net->addBus(bus_port, bus);
        }
        catch (int e) {
                // Now, it's entirely possible that we can't connect to a given
                // bus
            QString error = "Could not connect to bus: ";
            error.append(bus);
            error.append(":\n  ");

            ok = false;

            qDebug() << "UD:recoverBuses: Got error" << e;
            if( OM_ERR_SERAVAIL ) {
                error.append("Serial device" + bus_port + " not found");
            }
            else {
                error.append("Unknown error #");
                error.append(e);
            }
            ErrorDialog er(m_parent);
            er.setError(error);
            er.exec();
        }

         // get devices recovered as well (but only if we could connect to the bus)
        if( ok ) {
            _recoverDevices(p_net, bus, bus_port);
        }
    }

    m_qset->endGroup();
}

void UserData::_recoverDevices(OMNetwork *p_net, QString p_bus, QString p_busPort) {
    qDebug() << "UD:recoverBuses: Searching for devices in bus";

    m_qset->beginGroup(p_bus + "/" + "devices");
    QStringList devs = m_qset->childGroups();
    QString dev;

    foreach(dev, devs) {

        qDebug() << "UD:recoverBuses: Found a device at address " << dev.toUShort();

        // attempt to add each listed device
        QString this_root = dev + "/";
        try {
            p_net->addDevice(p_busPort, dev.toUShort(), m_qset->value(this_root + "type").toString(), m_qset->value(this_root + "name").toString());
        }
        catch(int e) {
                // We really shouldn't get errors trying to add previously-known
                // devices, but if someone changes their .ini file, all sorts of
                // bad things can happen!
            QString error = "Unable to add device at address " + dev + ":";

            if( e == OM_NET_DUPE )
                error.append("Address already in use");
            else if( e == OM_NET_DEV )
                error.append("Device type is not supported");
            else
                error.append("Uknown Error");

            ErrorDialog er(m_parent);
            er.setError(error);
            er.exec();

        } // end catch
    } // end foreach(dev...

    m_qset->endGroup(); // clear out the jump down into the devices
}


void UserData::deviceDeleted(OMbusInfo *p_bus, unsigned short p_addr) {
    qDebug() << "UD:deviceDeleted: Got Device" << p_bus->name << p_addr;

    QString key = "network/buses/" + p_bus->name + "/devices/" + QString::number(p_addr);
    m_qset->remove(key);
}


void UserData::busDeleted(QString p_port, QString p_name) {
    qDebug() << "UD:busDeleted: Got Bus" << p_port << p_name;

    QString key = "network/buses/" + p_name;
    m_qset->beginGroup(key);
    m_qset->remove("");
    m_qset->endGroup();
    m_qset->remove(key);
}

void UserData::deviceOptionsRemoved(unsigned short p_addr) {
    qDebug() << "UD:Got request to remove device options for" << p_addr;

    QString key = "film/device_options/" + QString::number(p_addr);
    m_qset->remove(key);
}

void UserData::_haveWritten(bool p_stat) {
    m_qset->setValue("stat", p_stat);
}
