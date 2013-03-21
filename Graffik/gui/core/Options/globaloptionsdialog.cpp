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

    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::FramelessWindowHint);

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

    m_opts->theme(ui->themeCombo->currentText());

        // Set new hotkeys
    QHash<int, QString> hotkeys;

    if( ui->hkPlayLine->text().length() > 0 )
        hotkeys.insert(HotKeys::FilmPlay, ui->hkPlayLine->text());
    if( ui->hkStopLine->text().length() > 0 )
        hotkeys.insert(HotKeys::FilmStop, ui->hkStopLine->text());
    if( ui->hkFwdLine->text().length() > 0 )
        hotkeys.insert(HotKeys::FilmFwd, ui->hkFwdLine->text());
    if( ui->hkRwdLine->text().length() > 0 )
        hotkeys.insert(HotKeys::FilmRwd, ui->hkRwdLine->text());
    if( ui->hkFFwdLine->text().length() > 0 )
        hotkeys.insert(HotKeys::FilmFFwd, ui->hkFFwdLine->text());
    if( ui->hkFRwdLine->text().length() > 0 )
        hotkeys.insert(HotKeys::FilmFRwd, ui->hkFRwdLine->text());

    m_opts->hotkeys(hotkeys);

    done(QDialog::Accepted);
}

void GlobalOptionsDialog::on_cancelButton_clicked() {
    done(QDialog::Rejected);
}

void GlobalOptionsDialog::on_clearButton_clicked() {
    emit optionsCleared();
    done(QDialog::Rejected);
}

void GlobalOptionsDialog::_setupInputs() {

    int dispTyp = m_opts->display();

        // populate theme list
    QString curTheme = m_opts->theme();

    Themer* themer = &Singleton<Themer>::Instance();
    QList<QString> themeList = themer->themes();

    for( int i = 0; i < themeList.length(); i++ ) {
        ui->themeCombo->addItem(themeList.at(i));
        if( themeList.at(i) == curTheme )
            ui->themeCombo->setCurrentIndex(i);
    }

    ui->stopCheckBox->setChecked(m_opts->stopOnErr());


    ui->displayCombo->addItem(GOD_IMP_LABEL);
    ui->displayCombo->addItem(GOD_MET_LABEL);
    ui->displayCombo->addItem(GOD_STP_LABEL);

    if( dispTyp == Options::Imperial )
        ui->displayCombo->setCurrentIndex(0);
    else if( dispTyp == Options::Metric )
        ui->displayCombo->setCurrentIndex(1);
    else if( dispTyp == Options::Steps)
        ui->displayCombo->setCurrentIndex(2);

        // restore hotkeys
    QHash<int, QString> hotkeys = m_opts->hotkeys();

    if( hotkeys.contains(HotKeys::FilmPlay) )
        ui->hkPlayLine->setText(hotkeys.value(HotKeys::FilmPlay));
    if( hotkeys.contains(HotKeys::FilmStop) )
        ui->hkStopLine->setText(hotkeys.value(HotKeys::FilmStop));
    if( hotkeys.contains(HotKeys::FilmFwd) )
        ui->hkFwdLine->setText(hotkeys.value(HotKeys::FilmFwd));
    if( hotkeys.contains(HotKeys::FilmRwd) )
        ui->hkRwdLine->setText(hotkeys.value(HotKeys::FilmRwd));
    if( hotkeys.contains(HotKeys::FilmFFwd) )
        ui->hkFFwdLine->setText(hotkeys.value(HotKeys::FilmFFwd));
    if( hotkeys.contains(HotKeys::FilmFRwd) )
        ui->hkFRwdLine->setText(hotkeys.value(HotKeys::FilmFRwd));

}
