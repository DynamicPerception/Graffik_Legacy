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

#include "axisoptions.h"

#include <QDebug>

AxisOptions::AxisOptions(QObject *parent) : QObject(parent) {

    // allow us to serialize OMaxisOptions structures
    // and use OMaxisOptions structures in arguments to
    // signals and slots

     qRegisterMetaType<OMaxisOptions>("OMaxisOptions");
     qRegisterMetaTypeStreamOperators<OMaxisOptions>("OMaxisOptions");

     m_optMutex = new QMutex(QMutex::Recursive);

}

AxisOptions::~AxisOptions() {

    OMaxisOptions* opt;
    foreach(opt, m_optList) {
        delete opt;
    }

    m_optList.clear();

    delete m_optMutex;
}

/** Get Options for a Specific Axis

  Retrieves option data for a specific axis

  @return
  Pointer to OMaxisOptions structure
  */

OMaxisOptions* AxisOptions::getOptions(unsigned short p_addr) {

    m_optMutex->lock();

    if( ! m_optList.contains(p_addr) ) {
        m_optMutex->unlock();
        return 0;
    }

    OMaxisOptions* ret = m_optList.value(p_addr);

    m_optMutex->unlock();

    return ret;
}

/** Set Options for a Specific Axis

  Sets option data for a specific axis

  @param p_addr
  The address of the axis

  @param p_opts
  A pointer to an OMaxisOptions structure.  While you need to create it, you do
  not need to manage the lifetime of the pointer, and should always use the
  getOptions() method to retrieve it.  This class wil manage the life of the
  created structure for you.

  */

void AxisOptions::setOptions(unsigned short p_addr, OMaxisOptions *p_opts) {
    m_optMutex->lock();
    m_optList[p_addr] = p_opts;
    m_optMutex->unlock();
    emit deviceOptionsChanged(p_opts, p_addr);
}



/** Set a Specific Axis as Timing Master

  Since only one device may be a timing master on the bus, when changing one device
  to be the timing master, we need to change all others to be slaves.  This
  method does this for you automatically.  Note, however, this only affects the
  stored configuration for the node devices, and does not transmit new data
  to them.  You still need to do this separately from this class.

  @param p_addr
  Address of the device that shall be the timing master
  */

void AxisOptions::setMaster(unsigned short p_addr) {
    // only one axis can be a timing master

    qDebug() << "OAFO: setMaster called for" << p_addr;

    m_optMutex->lock();

    QList<unsigned short> addresses = m_optList.keys();
    unsigned short addr;

    foreach(addr, addresses) {
        if(addr == p_addr) {
            qDebug() << "OAFO: setMaster: Found options for" << p_addr;
            m_optList[addr]->master = true;
            emit deviceOptionsChanged(m_optList[addr], addr);
        }
        else if(m_optList[addr]->master == true ) {
                // only update the previous master, if there is one
            m_optList[addr]->master = false;
            emit deviceOptionsChanged(m_optList[addr], addr);
        }
    }

    m_optMutex->unlock();
}

void AxisOptions::deviceAdded(OMbusInfo*, OMdeviceInfo *p_dev) {
    qDebug() << "OAFO: Added new device" << p_dev->device->address();
        // only create new settings if they do not already exist

    m_optMutex->lock();

    if( ! m_optList.contains(p_dev->device->address()) )
        setOptions(p_dev->device->address(), new OMaxisOptions);

    m_optMutex->unlock();
}

void AxisOptions::deviceDeleted(OMbusInfo*, unsigned short p_addr) {
    qDebug() << "OAFO: Deleted device" << p_addr;

    m_optMutex->lock();

    if( m_optList.contains(p_addr) ) {
        OMaxisOptions* ptr = m_optList[p_addr];
        delete ptr;
        m_optList.remove(p_addr);
    }

    m_optMutex->unlock();

    emit deviceOptionsRemoved(p_addr);
}

