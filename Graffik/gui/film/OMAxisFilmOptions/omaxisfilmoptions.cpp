#include "omaxisfilmoptions.h"

#include <QDebug>

OMAxisFilmOptions::OMAxisFilmOptions(QObject *parent) :
    QObject(parent)
{
}

OMAxisFilmOptions::~OMAxisFilmOptions() {

    OMaxisOptions* opt;
    foreach(opt, m_optList) {
        delete opt;
    }

    m_optList.clear();

}


OMaxisOptions* OMAxisFilmOptions::getOptions(unsigned short p_addr) {
    if( m_optList.contains(p_addr) )
        return m_optList[p_addr];
    else
        return 0;
}

void OMAxisFilmOptions::setOptions(unsigned short p_addr, OMaxisOptions *p_opts) {
    m_optList[p_addr] = p_opts;

}


void OMAxisFilmOptions::deviceAdded(OMbusInfo *p_bus, OMdeviceInfo *p_dev) {
    qDebug() << "OAFO: Added new device" << p_dev->device->address();
    m_optList[p_dev->device->address()] = new OMaxisOptions;
}


