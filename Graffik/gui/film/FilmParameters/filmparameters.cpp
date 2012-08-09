#include "filmparameters.h"
#include <QDebug>

FilmParameters::FilmParameters(OMNetwork *c_net, QWidget *parent) : QWidget(parent) {
    m_net = c_net;
    m_filmParams = new OMfilmParams;

    m_mutex = new QMutex(QMutex::NonRecursive);

    QObject::connect(m_net, SIGNAL(deviceAdded(OMbusInfo*, OMdeviceInfo*)), this, SLOT(_addDevice(OMbusInfo*, OMdeviceInfo*)));
    QObject::connect(m_net, SIGNAL(deviceDeleted(QString,unsigned short)), this, SLOT(_removeDevice(QString,unsigned short)));

}

FilmParameters::~FilmParameters() {

     if( m_isLocked )
         m_mutex->unlock();

    delete m_filmParams->camParams;

    foreach(unsigned short i, m_filmParams->axes.keys()) {
        delete m_filmParams->axes.value(i);
        m_filmParams->axes.remove(i);
    }

    delete m_filmParams;

    delete m_mutex;
}

 /** Add information about a new device to the film structure */

void FilmParameters::_addDevice(OMbusInfo *p_bus, OMdeviceInfo *p_dev) {

    qDebug() << "FP: Got new axis" << p_dev->name;

    m_mutex->lock();

    OMfilmAxisParams* axis = new OMfilmAxisParams;
        // record bus (port) name of device
    axis->bus = p_bus->bus->port();

    m_filmParams->axes.insert(p_dev->device->address(), axis);

    m_mutex->unlock();
}

void FilmParameters::_removeDevice(QString p_bus, unsigned short p_addr) {

    qDebug() << "FP: Removing axis" << p_bus << p_addr;

    m_mutex->lock();

    if( m_filmParams->axes.contains(p_addr) ) {
        delete m_filmParams->axes.value(p_addr);
        m_filmParams->axes.remove(p_addr);
    }

    m_mutex->unlock();
}

/** Get Film Parameters Structure

  This method returns a pointer to the current film parameters structure,
  and locks out any further access to the parameters structure.  You must
  call releaseParams() when done doing any reading or making any changes.

  You must also do this fairly quickly, everyone else will block while trying
  to access it - including any function that adds or removes a device!

  If you only intend to read the data currently stored, and don't care if it
  changes while you're reading it, then use the getParamsCopy() method that
  returns a copy of the film parameters structure.

  @return
  A pointer to the current film parameters structure
  */

OMfilmParams* FilmParameters::getParams() {
    m_mutex->lock();
    return m_filmParams;
}

OMfilmParams FilmParameters::getParamsCopy() {

    m_mutex->lock();
    OMfilmParams ret = *m_filmParams;
    m_mutex->unlock();
    return ret;

}

/** Removes Hold on Structure

  Using the form of getParams() that returns a pointer to the parameters
  object places a lock on the structure until you're done modifying it.

  Indicate that you're done modifying it by calling this method.
  */
void FilmParameters::releaseParams() {
    m_mutex->unlock();
    qDebug() << "FP: Releasing Params";
    emit paramsReleased();
}
