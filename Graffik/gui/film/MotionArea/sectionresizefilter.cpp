#include "sectionresizefilter.h"

#include <QEvent>
#include <QDebug>

SectionResizeFilter::SectionResizeFilter(MotionSection *c_motion, QObject *parent) :
    QObject(parent)
{
    m_motion = c_motion;
    qDebug() << "SRF INIT: " << c_motion;

}


bool SectionResizeFilter::eventFilter(QObject *p_obj, QEvent *p_event) {

    if( p_event->type() == QEvent::Resize ) {
        qDebug() << "SRF: got resize" << p_obj;
        qDebug() << "SRF: " << m_motion;


        QWidget *parent = m_motion->parentWidget();

        qDebug() << "SRF: got resize" << parent;

        m_motion->setGeometry(parent->rect());
        return false;
    }
    return QObject::eventFilter( p_obj, p_event );
}
