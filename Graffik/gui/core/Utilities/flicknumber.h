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

#ifndef FLICKNUMBER_H
#define FLICKNUMBER_H

#include <QWidget>
#include <QString>
#include <QResizeEvent>
#include <QScrollBar>
#include <QListWidgetItem>

#include "flickcharm.h"


namespace Ui {
class FlickNumber;
}

/** FlickNumber Class

  Provides a Widget type that is a flickable (kinetic scrolling) number input

  @author
  C. A. Church

  */

class FlickNumber : public QWidget
{
    Q_OBJECT
    
public:
    explicit FlickNumber(QWidget *parent = 0);
    FlickNumber(unsigned int c_count, QWidget *parent = 0);
    ~FlickNumber();

    void setCount(unsigned int p_count);
    void setValue(unsigned int p_val);
    unsigned int value();
    
    void resizeEvent(QResizeEvent* p_evt);
    void setEnabled(bool p_en);

public slots:
    void flickSettled();


signals:
    void valueChanged(unsigned int p_val);

private:
    Ui::FlickNumber *ui;
    FlickCharm* m_fcp;

    unsigned int m_cnt;

    void _setFlick();
};

#endif // FLICKNUMBER_H
