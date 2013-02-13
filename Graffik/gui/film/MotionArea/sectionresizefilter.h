/*

  Graffik Motion Control Application

  Copyright (c) 2011-2013 Dynamic Perception

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

#ifndef SECTIONRESIZEFILTER_H
#define SECTIONRESIZEFILTER_H

#include <QObject>
#include <QWidget>
#include "motionsection.h"

/** Section Resize Event Filter

  The Section Resize Event Filter is used to capture and filter update events for the
  re-drawing of the Motion Section Widget

  @author
  C. A. Church
  */

class SectionResizeFilter : public QObject
{
    Q_OBJECT
public:
    SectionResizeFilter(MotionSection* c_motion, QObject *parent = 0);
    
signals:
    
public slots:
    
protected:
    bool eventFilter(QObject *p_obj, QEvent *p_event);

private:
    MotionSection* m_motion;
};

#endif // SECTIONRESIZEFILTER_H
