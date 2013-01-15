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

#ifndef GLOBALOPTIONSDIALOG_H
#define GLOBALOPTIONSDIALOG_H

#include <QDialog>

#include "globaloptions.h"

#include "core/Themer/themer.h"
#include "core/Utilities/singleton.h"
#include "core/Themer/singlethemer.h"

#define GOD_IMP_LABEL   "Imperial"
#define GOD_MET_LABEL   "Metric"
#define GOD_STP_LABEL   "Steps"

namespace Ui {
    class GlobalOptionsDialog;
}

/** Global Options Dialog Class

  This dialog allows the user to manage options for the entire program

  @author
  C. A. Church
  */
class GlobalOptionsDialog : public QDialog {
    Q_OBJECT
    
public:
    GlobalOptionsDialog(GlobalOptions* c_opts, QWidget *parent = 0);
    ~GlobalOptionsDialog();

public slots:
    void on_okButton_clicked();
    void on_cancelButton_clicked();
    void on_clearButton_clicked();

signals:

    void optionsCleared();

private:
    Ui::GlobalOptionsDialog *ui;

    GlobalOptions* m_opts;

    void _setupInputs();

};

#endif // GLOBALOPTIONSDIALOG_H
