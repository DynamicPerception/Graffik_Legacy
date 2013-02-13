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

#ifndef WELCOMEDIALOG_H
#define WELCOMEDIALOG_H

#include <QDialog>
#include "core/Themer/singlethemer.h"


namespace Ui {
class WelcomeDialog;
}

/** Welcome Dialog Class

  Presents a welcome dialog, primarily used for the first time a user
  runs the application, or the first run after clearing settings.

  @author
  C. A. Church
  */

class WelcomeDialog : public QDialog {
    Q_OBJECT
    
public:
    explicit WelcomeDialog(QWidget *parent = 0);
    ~WelcomeDialog();
    
public slots:
    void on_cancelButton_clicked();
    void on_addButton_clicked();

signals:
    /** User Clicked Add Bus button */
    void addBus();

private:
    Ui::WelcomeDialog *ui;
};

#endif // WELCOMEDIALOG_H
