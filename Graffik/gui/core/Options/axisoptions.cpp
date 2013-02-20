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

/** Set an Error for a Specific Axis

  As there may be a need for recording an error with an axis, the AxisOptions
  class provides a centralized facility for storing and retrieving them.

  These errors are not part of the OMaxisOptions structure, and are not
  persisted to disk when the structure is persisted.  They are for run-time
  use only.

  You must use one of the types defined in the AxisErrors namespace

  @param p_addr
  The address of the axis

  @param p_err
  The error code
  */

void AxisOptions::error(unsigned short p_addr, int p_err) {
    m_optMutex->lock();

    if( ! m_optList.contains(p_addr) ) {
        m_optMutex->unlock();
        return;
    }

    QList<unsigned short> keys = m_errList.keys();

    if( ! keys.contains(p_addr) ) {
        QList<int> errs;
        errs.append(p_err);
        m_errList.insert(p_addr, errs);
    }
    else {
        QList<int> errs = m_errList.value(p_addr);
        errs.append(p_err);
        m_errList.insert(p_addr, errs);
    }

    m_optMutex->unlock();
}

/** Remove a Specific Error Code from a Specific Axis

  Removes all instances of a specific error code from an
  axis.

  @param p_addr
  The address of the axis

  @param p_err
  The error code
  */

void AxisOptions::removeError(unsigned short p_addr, int p_err) {
    m_optMutex->lock();

    if( ! m_optList.contains(p_addr) ) {
        m_optMutex->unlock();
        return;
    }

    QList<unsigned short> keys = m_errList.keys();

    if( ! keys.contains(p_addr) ) {
        return;
    }
    else {
        QList<int> errs = m_errList.value(p_addr);
        errs.removeAll(p_err);
        m_errList.insert(p_addr, errs);
    }

    m_optMutex->unlock();
}

/** Get All Errors for a Specific Axis as Strings

  Returns a copy of all errors recorded for a specific axis.

  This method returns human-readable strings for the errors.

  @param p_addr
  The address of the axis

  @return
  A QList containing all error strings
  */

QList<QString> AxisOptions::errors(unsigned short p_addr) {
    m_optMutex->lock();

    using namespace AxisErrors;

    QList<QString> errors;

    if( ! m_optList.contains(p_addr) ) {
        m_optMutex->unlock();
        return errors;
    }

    QList<unsigned short> keys = m_errList.keys();

    if( keys.contains(p_addr) ) {

        QList<int> errInts = m_errList.value(p_addr);

        foreach(int e, errInts) {
            errors.append(_errToString(e));
        }
    }

    m_optMutex->unlock();

    return errors;
}


/** Clears all Errors for a Specific Axis

  @param p_addr
  The address of the axis to clear errors for
  */

void AxisOptions::clearErrors(unsigned short p_addr) {
    m_optMutex->lock();

    if( ! m_optList.contains(p_addr) ) {
        m_optMutex->unlock();
        return;
    }

    QList<unsigned short> keys = m_errList.keys();

    if( keys.contains(p_addr) ) {
       m_errList.remove(p_addr);
    }

    m_optMutex->unlock();
}

QString AxisOptions::_errToString(int p_err) {
    using namespace AxisErrors;
    switch(p_err) {
        case ErrorSpeedExceeded:
            return ERR_STR_SPD;
            break;
        case ErrorNoInterval:
            return ERR_STR_NFI;
            break;
        case ErrorNoTime:
            return ERR_STR_NET;
            break;
        case ErrorIntervalSpeed:
            return ERR_STR_NIT;
            break;
        default:
            return QString("Unknown Error");
            break;
    }
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

    QList<unsigned short>  addresses = m_optList.keys();
    QList<unsigned short> bCastAddrs;

    unsigned short addr;


    foreach(addr, addresses) {
        if(addr == p_addr) {
            qDebug() << "OAFO: setMaster: Found options for" << p_addr;
            m_optList[addr]->master = true;
            bCastAddrs.append(addr);
        }
        else if(m_optList[addr]->master == true ) {
                // only update the previous master, if there is one
            m_optList[addr]->master = false;
            bCastAddrs.append(addr);
        }
    }

    m_optMutex->unlock();

    foreach( unsigned short addr, bCastAddrs)
        emit deviceOptionsChanged(m_optList[addr], addr);

}

void AxisOptions::deviceAdded(OMbusInfo*, OMdeviceInfo *p_dev) {
    qDebug() << "OAFO: Added new device" << p_dev->device->address();
        // only create new settings if they do not already exist

    m_optMutex->lock();

    if( ! m_optList.contains(p_dev->device->address()) )
        setOptions(p_dev->device->address(), new OMaxisOptions);

        // if only first device, set it as master by default.
    if( m_optList.count() == 1 ) {
        OMaxisOptions* opts = getOptions(p_dev->device->address());
        opts->master = true;
    }

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

