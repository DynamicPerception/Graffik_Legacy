#include "sectionresizefilter.h"

#include <QEvent>
#include <QDebug>

SectionResizeFilter::SectionResizeFilter(MotionSection *c_motion, QObject *parent) :
    QObject(parent)
{

    qDebug() << "SRF INIT: " << c_motion;

}


bool SectionResizeFilter::eventFilter(QObject *p_obj, QEvent *p_event) {

    if( p_event->type() == QEvent::Resize ) {
        qDebug() << "SRF: got resize" << p_obj;
        qDebug() << "SRF: " << m_motion;
        return false;

        QWidget *parent = m_motion->parentWidget();

        if( parent == 0 ) {
            qDebug() << "SRF: No parent?";
            return false;
        }

        qDebug() << "SRF: got resize" << parent;

        // m_motion->setGeometry(parent->rect());
        return false;
    }
    return QObject::eventFilter( p_obj, p_event );
}
