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

#include "globaloptionsdialog.h"
#include "ui_globaloptionsdialog.h"

GlobalOptionsDialog::GlobalOptionsDialog(GlobalOptions *c_opts, QWidget *parent) : QDialog(parent), ui(new Ui::GlobalOptionsDialog) {
    ui->setupUi(this);

    m_opts = c_opts;

    _setupInputs();

    setStyleSheet(SingleThemer::getStyleSheet("global_opts"));
}

GlobalOptionsDialog::~GlobalOptionsDialog() {
    delete ui;
}

void GlobalOptionsDialog::on_okButton_clicked() {
    bool stop = ui->stopCheckBox->isChecked();
    int disp  = ui->displayCombo->currentIndex();

    m_opts->stopOnErr(stop);

    if( disp == 0 )
        m_opts->display(Options::Imperial);
    else if( disp == 1 )
        m_opts->display(Options::Metric);
    else if( disp == 2 )
        m_opts->display(Options::Steps);

    this->done(QDialog::Accepted);
}

void GlobalOptionsDialog::on_cancelButton_clicked() {
    this->done(QDialog::Rejected);
}

void GlobalOptionsDialog::_setupInputs() {

    ui->stopCheckBox->setChecked(m_opts->stopOnErr());
    int dispTyp = m_opts->display();

    ui->displayCombo->addItem(GOD_IMP_LABEL);
    ui->displayCombo->addItem(GOD_MET_LABEL);
    ui->displayCombo->addItem(GOD_STP_LABEL);

    if( dispTyp == Options::Imperial )
        ui->displayCombo->setCurrentIndex(0);
    else if( dispTyp == Options::Metric )
        ui->displayCombo->setCurrentIndex(1);
    else if( dispTyp == Options::Steps)
        ui->displayCombo->setCurrentIndex(2);

}
