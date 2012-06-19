#include "filmparameters.h"
#include <QDebug>

FilmParameters::FilmParameters(OMNetwork *c_net, QWidget *parent) :
    QWidget(parent)
{
    m_net = c_net;
    m_filmParams = new OMfilmParams;

    QObject::connect(m_net, SIGNAL(deviceAdded(OMdeviceInfo*)), this, SLOT(_addDevice(OMdeviceInfo*)));
    QObject::connect(m_net, SIGNAL(deviceDeleted(QString,unsigned short)), this, SLOT(_removeDevice(QString,unsigned short)));

}

/* FilmParameters::~FilmParameters() {
    delete m_filmParams->camParams;

    foreach(unsigned short i, m_filmParams->axes.keys()) {
        delete m_filmParams->axes.value(i);
        m_filmParams->axes.remove(i);
    }

    delete m_filmParams;
}*/

void FilmParameters::_addDevice(OMdeviceInfo *p_dev) {

    qDebug() << "FP: Got new axis" << p_dev->name;

    OMfilmAxisParams* axis = new OMfilmAxisParams;
    m_filmParams->axes.insert(p_dev->device->address(), axis);
}

void FilmParameters::_removeDevice(QString p_bus, unsigned short p_addr) {

    qDebug() << "FP: Removing axis" << p_bus << p_addr;

    if( m_filmParams->axes.contains(p_addr) ) {
        delete m_filmParams->axes.value(p_addr);
        m_filmParams->axes.remove(p_addr);
    }
}
