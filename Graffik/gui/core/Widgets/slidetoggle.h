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

#ifndef SLIDETOGGLE_H
#define SLIDETOGGLE_H

#include <QSlider>

/** SlideToggle Class

  A user input widget that provides a binary, left-to-right sliding on/off toggle.

  @author
  C. A. Church
  */

class SlideToggle : public QSlider {
    Q_OBJECT

public:
    explicit SlideToggle(QWidget *parent = 0);
    SlideToggle(Qt::Orientation orientation, QWidget *parent = 0);

signals:
    void selected(int p_val);

public slots:

private slots:
    void _getValue(int p_val);

private:
    void _setup();

};

#endif // SLIDETOGGLE_H
