#include "sectionresizefilter.h"

#include <QEvent>
#include <QDebug>

SectionResizeFilter::SectionResizeFilter(MotionSection *c_motion, QObject *parent) :
    QObject(parent)
{
    m_motion = c_motion;

}


bool SectionResizeFilter::eventFilter(QObject *p_obj, QEvent *p_event) {

    if( p_event->type() == QEvent::Resize ) {
        QWidget* parent = m_motion->parentWidget();

        m_motion->setGeometry(parent->rect());
        return false;
    }
    else if( p_event->type() == QEvent::Paint ) {
        m_motion->update();
        return false;
    }
    return QObject::eventFilter( p_obj, p_event );
}
