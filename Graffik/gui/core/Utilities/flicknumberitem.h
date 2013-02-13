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

#ifndef FLICKNUMBERITEM_H
#define FLICKNUMBERITEM_H

#include <QWidget>
#include <QListWidgetItem>
#include <QString>
#include <QLabel>
#include <QHBoxLayout>

class FlickNumberItem : public QWidget
{

    Q_OBJECT

public:
    explicit FlickNumberItem(QString c_first, QString c_second, QWidget *parent = 0);
    ~FlickNumberItem();
    
    //QWidget* widget();

    void setFirst(QString p_str);
    void setSecond(QString p_str);

private:
   // QWidget*     m_widget;
    QLabel*      m_first;
    QLabel*      m_second;
    QHBoxLayout* m_layout;
};

#endif // FLICKNUMBERITEM_H
