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

#include "jogcontrolpanel.h"
#include "ui_jogcontrolpanel.h"

#include <QDebug>

JogControlPanel::JogControlPanel(OMNetwork *c_net, AxisOptions* c_opts, FilmParameters *c_params, QWidget *parent) : QWidget(parent), ui(new Ui::JogControlPanel) {
    ui->setupUi(this);

    m_net = c_net;
    m_opts = c_opts;
    m_params = c_params;

    m_ldModel = new LiveDeviceModel(m_net, this);
    m_jcm = new JogControlManager(m_net, m_opts, m_ldModel, ui->jogDial, ui->jogSpeedSlider, ui->jogDampSlider, ui->jogHomeButton, ui->jogEndButton, this);

        // connect the device list display to the live device model
    ui->devButtonList->setModel(m_ldModel);

        // pass a click on to the model via signal
    connect(ui->devButtonList, SIGNAL(clicked(const QModelIndex &)), m_ldModel, SLOT(deviceClicked(const QModelIndex &)));

    connect(m_jcm, SIGNAL(motorChangeDenied(unsigned short)), this, SLOT(_jogMotorChangeDenied(unsigned short)));
    connect(m_jcm, SIGNAL(endPosition(unsigned short,long)), this, SLOT(_endSet(unsigned short,long)));

    // theming

    Themer* theme = &Singleton<Themer>::Instance();
    connect(theme, SIGNAL(themeChanged()), this, SLOT(_themeChanged()));

    setStyleSheet(theme->getThemeCSS("jog"));

}

JogControlPanel::~JogControlPanel()
{
    delete ui;
    delete m_jcm;
    delete m_ldModel;
}


void JogControlPanel::_themeChanged() {
    setStyleSheet(SingleThemer::getStyleSheet("jog"));
    style()->unpolish(this);
    style()->polish(this);
    update();
}

void JogControlPanel::_jogMotorChangeDenied(unsigned short p_oldAddr) {
    qDebug() << "JCP: Motor change was denied, re-setting devButtonList selection" << p_oldAddr;

    int row = m_ldModel->find(p_oldAddr);

    QModelIndex tl = m_ldModel->index(row, 0, QModelIndex());
    QModelIndex tr = m_ldModel->index(row, 0, QModelIndex());
    QItemSelection sel(tl, tr);
    ui->devButtonList->selectionModel()->select(sel, QItemSelectionModel::SelectCurrent);
}


void JogControlPanel::_endSet(unsigned short p_addr, long p_dist) {
    qDebug() << "JCP: Got EndSet Signal" << p_addr << p_dist;

    OMfilmParams* parms = m_params->getParams();

    OMfilmAxisParams* axis = parms->axes.value(p_addr);
    axis->endDist = p_dist;

        // TODO: Replace hard-coded percentages
    if( axis->accelTm == 0 )
        axis->accelTm = ( (float)parms->realLength * 0.10);

    if( axis->decelTm == 0 )
        axis->decelTm = ( (float)parms->realLength * 0.10);


    m_params->releaseParams();
}
