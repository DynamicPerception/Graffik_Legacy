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

#ifndef HELPWINDOW_H
#define HELPWINDOW_H

#include <QMainWindow>
#include <QtHelp/QHelpEngine>
#include "helpbrowser.h"

namespace Ui {
class HelpWindow;
}

class HelpWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit HelpWindow(QWidget *parent = 0);
    ~HelpWindow();
    
public slots:
    void on_closeButton_clicked();

private:
    Ui::HelpWindow *ui;
    QHelpEngine* m_help;
    HelpBrowser* m_browse;
};

#endif // HELPWINDOW_H
