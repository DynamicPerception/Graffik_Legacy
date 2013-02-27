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

#ifndef MOTIONTAPE_H
#define MOTIONTAPE_H

#include <QWidget>
#include <QString>

#include "film/FilmParameters/filmparameters.h"

#include <QPainterPath>
#include <QPainter>
#include <QPaintEvent>
#include <QRect>
#include <QFont>
#include <QWidget>
#include <QColor>
#include <QPixmap>
#include <QPoint>

const int MT_LINE_MINSPC = 5;
const int MT_FONT_SIZE   = 8;

const QString MT_LABEL_SS = "SS";
const QString MT_LABEL_MM = "MM";
const QString MT_LABEL_HH = "HH";
const QString MT_LABEL_DD = "DD";

const QString MT_LABEL_FONT =  "Courier";


namespace Ui {
class MotionTape;
}

/** Motion Tape Widget

  The Motion Tape Widget displays the timeline tape the top of the motion section,
  showing ticks and labels as needed.

  @author
  C. A. Church
  */

class MotionTape : public QWidget
{
    Q_OBJECT
        /** Background color */
    Q_PROPERTY(QColor background READ background WRITE setBackground DESIGNABLE true)
        /** Color of tick lines */
    Q_PROPERTY(QColor color READ color WRITE setColor DESIGNABLE true)
        /** Icon for current position indicator */
    Q_PROPERTY(QPixmap icon READ icon WRITE setIcon DESIGNABLE true)

public:
    explicit MotionTape(FilmParameters* c_film, QWidget* c_scroll, QWidget *parent = 0);
    ~MotionTape();

    void paintEvent(QPaintEvent* p_event);
    
    QColor background();
    void setBackground(QColor p_col);

    QPixmap icon();
    void setIcon(QPixmap p_icon);

    QColor color();
    void setColor(QColor p_col);

    void mousePressEvent(QMouseEvent* p_event);

signals:
    void timelineClick(unsigned long p_time);

public slots:

    void filmUpdated();
    void setBorders(int p_left, int p_right);
    void disableClicks(bool p_en);

    void timeChanged(unsigned long p_time);

private:
    Ui::MotionTape* ui;
    QPainterPath* m_path;
    FilmParameters* m_film;
    QFont* m_font;
    QWidget* m_scroll;
    QPixmap* m_icon;

    bool m_drawn;
    bool m_enableClicks;
    unsigned long m_time;
    unsigned long m_curTime;
    unsigned long m_wasTime;
    int m_width;
    int m_scrollWidth;
    int m_preSpace;
    int m_leftX;
    int m_rightX;
    int m_iconX;
    int m_iconY;

    QColor m_bgCol;
    QColor m_fgCol;

    void _drawTime(QRect p_rect);
    void _drawLines(QRect p_rect, unsigned int p_lines, int p_height, int p_fill = 0, int p_pad = 0);
    float _calcSpacing(QRect p_rect, int p_lines, int p_fill = 0, int p_pad = 0);

};

#endif // MOTIONTAPE_H
