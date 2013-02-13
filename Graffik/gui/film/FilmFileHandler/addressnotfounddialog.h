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

#ifndef ADDRESSNOTFOUNDDIALOG_H
#define ADDRESSNOTFOUNDDIALOG_H

#include <QDialog>
#include <QHash>


namespace Ui {
class AddressNotFoundDialog;
}

/** Address Not Found Dialog Class

  This dialog is displayed for each address found in a film file, which
  is not present in the current network and allows the user to re-map it
  to a known device address.

  @author
  C. A. Church
  */

class AddressNotFoundDialog : public QDialog
{
    Q_OBJECT
    
public:
    AddressNotFoundDialog(unsigned short c_addr, QHash<unsigned short, QString> c_list, QWidget *parent = 0);
    ~AddressNotFoundDialog();

    void accept();
    
private:
    Ui::AddressNotFoundDialog *ui;

    QHash<unsigned short, QString> m_replace;
};

#endif // ADDRESSNOTFOUNDDIALOG_H
