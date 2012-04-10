#include "userdata.h"

#include <QCoreApplication>

UserData::UserData()
{
    QCoreApplication::setOrganizationName("Dynamic Perception");
    QCoreApplication::setOrganizationDomain("dynamicperception.com");
    QCoreApplication::setApplicationName("Graffik");

    m_qset = new QSettings(QSettings::IniFormat, QSettings::UserScope, "Dynamic Perception", "Graffik");
}

UserData::~UserData() {
    delete m_qset;
}

UserData::busAdded(OMbusInfo *p_bus) {
    QHash<QString, QVariant> busDef;
    QStringList


}
