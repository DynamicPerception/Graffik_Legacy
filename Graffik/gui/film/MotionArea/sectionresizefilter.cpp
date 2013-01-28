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
    else if( p_event->type() == QEvent::Show ) {
        m_motion->update();
        return false;
    }

    return QObject::eventFilter( p_obj, p_event );
}
