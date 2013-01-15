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

#include "motionsection.h"

#include <QDebug>
#include <QLayout>

MotionSection::MotionSection(FilmExec *c_exec, FilmParameters *c_film, QWidget *parent) : QWidget(parent) {
    m_path = new QPainterPath;
    m_film = c_film;
    m_exec = c_exec;
    m_parent = parent;

    m_width  = 0;
    m_height = 0;
    m_curPos = 0;
    m_wasPos = 0;
    m_length = 0;

    m_scrollWidth = m_parent->width() - m_parent->layout()->contentsMargins().right();

        // we're making a transparent widget, to overlay the section and draw over all children
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_TranslucentBackground);

    connect(m_film, SIGNAL(paramsReleased()), this, SLOT(paramsChanged()));
    connect(m_exec, SIGNAL(filmPlayStatus(bool,ulong)), this, SLOT(playStatusChanged(bool,ulong)));

    m_leftX = 0;
    m_rightX = rect().width();

}

MotionSection::~MotionSection() {
    delete m_path;
}

void MotionSection::paintEvent(QPaintEvent * p_event) {
     QPainter painter(this);

     QRect curRect = rect();

     if( m_width != (m_rightX - m_leftX) ||
            m_curPos != m_wasPos ) {

        m_width  = m_rightX - m_leftX;
        m_height = curRect.height();
        m_wasPos = m_curPos;
        _updatePath();
    }

    painter.drawPath(*m_path);

}


void MotionSection::paramsChanged() {

    OMfilmParams params = m_film->getParamsCopy();

    if( params.realLength == m_length )
        return;

    m_length = params.realLength;
    _updatePath();
    this->update();
}


void MotionSection::playStatusChanged(bool p_stat, unsigned long p_runTime) {

    if( p_stat == false )
        return;

    m_curPos = p_runTime;
    this->update();
}

void MotionSection::_updatePath() {

    delete m_path;
    m_path = new QPainterPath;

    int curXpos = (((float) m_curPos / (float) m_length) * (float) m_width);

    m_path->moveTo(m_leftX + curXpos, 0);
    m_path->lineTo(m_leftX + curXpos, m_height);
}


void MotionSection::setBorders(int p_left, int p_right) {
    m_leftX = mapFromGlobal(QPoint(p_left, 0)).x();
    m_rightX = mapFromGlobal(QPoint(p_right, 0)).x();

}


