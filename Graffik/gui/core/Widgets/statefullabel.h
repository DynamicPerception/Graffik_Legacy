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

#ifndef STATEFULLABEL_H
#define STATEFULLABEL_H

#include <QLabel>

/** Stateful Label

  A QLabel with a configurable state that can be accessed via CSS.

  */
class StatefulLabel : public QLabel
{
    Q_OBJECT
    /** State Property

      Set a state property that can be read/written via CSS.  Default value
      is 0.
      */
    Q_PROPERTY(int state READ state WRITE setState DESIGNABLE true)
public:
    explicit StatefulLabel(QWidget *parent = 0);
    int state();
    void setState(int p_state);

signals:
    
public slots:

private:
    int m_state;
};

#endif // STATEFULLABEL_H
