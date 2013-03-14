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

    m_net     = c_net;
    m_opts    = c_opts;
    m_params  = c_params;
    m_curMode = false;
    m_curAddr = 0;

    m_ldModel = new LiveDeviceModel(m_net, this);
    m_jcm     = new JogControlManager(m_net, m_opts, m_ldModel, this);

        // connect the device list display to the live device model
    ui->devButtonList->setModel(m_ldModel);

        // pass a click on to the model via signal
    connect(ui->devButtonList, SIGNAL(clicked(const QModelIndex &)), m_ldModel, SLOT(deviceClicked(const QModelIndex &)));

        // reflect the eStop signal here, so it can be captured upstream as well
    connect(ui->stopButton, SIGNAL(clicked()), this, SLOT(_eStop()));
    connect(this, SIGNAL(emergencyStop()), m_jcm, SLOT(emergencyStop()));

    connect(ui->modeButton, SIGNAL(clicked()), this, SLOT(_modeClicked()));
    connect(ui->jogDial, SIGNAL(mouseReleased()), this, SLOT(_dialReleased()));

        // listen to important signals from the JCM
    connect(m_jcm, SIGNAL(motorChangeDenied(unsigned short)), this, SLOT(_jogMotorChangeDenied(unsigned short)));
    connect(m_jcm, SIGNAL(motorChangeAllowed(unsigned short)), this, SLOT(_jogMotorChangeAllowed(unsigned short)));
    connect(m_jcm, SIGNAL(endPosition(unsigned short,long)), this, SLOT(_endSet(unsigned short,long)));
    connect(m_jcm, SIGNAL(motorStarted(unsigned short)), this, SLOT(_motorStarted(unsigned short)));
    connect(m_jcm, SIGNAL(motorStopped(unsigned short)), this, SLOT(_motorStopped(unsigned short)));

        // forward this signal, so that we can inform the JCM when someone smashes the stop or play button!
    connect(this, SIGNAL(playStatusChange(bool)), m_jcm, SLOT(playStatusChange(bool)));

        // forward ui input signals to the JCM
    connect(ui->jogHomeButton, SIGNAL(clicked()), m_jcm, SLOT(setHome()));
    connect(ui->jogEndButton, SIGNAL(clicked()), m_jcm, SLOT(setEnd()));
    connect(ui->jogDampSlider, SIGNAL(valueChanged(int)), m_jcm, SLOT(jogDampChange(int)));
    connect(ui->jogSpeedSlider, SIGNAL(valueChanged(int)), m_jcm, SLOT(jogMaxSpeedChange(int)));
    connect(ui->jogDial, SIGNAL(valueChanged(int)), m_jcm, SLOT(speedChange(int)));

    connect(ui->jogHomeButton, SIGNAL(clicked()), this, SLOT(_homeSet()));

    // OSX has issues laying out these buttons w/o overlap,
    // this is a work-around
#ifdef Q_WS_MAC
    ui->stopButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);
    ui->jogHomeButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);
    ui->jogEndButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);
    ui->modeButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);
    ui->ledHomeButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);
    ui->ledEndButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);
    ui->ledMovingButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);
#endif

    // theming

    Themer* theme = &Singleton<Themer>::Instance();
    connect(theme, SIGNAL(themeChanged()), this, SLOT(_themeChanged()));

    setStyleSheet(theme->getThemeCSS("jog"));

}

JogControlPanel::~JogControlPanel() {
    delete ui;
    delete m_jcm;
    delete m_ldModel;
}


void JogControlPanel::_motorStarted(unsigned short p_addr) {
    Q_UNUSED(p_addr);

    ui->ledMovingButton->setState(1);
    Themer::rePolish(ui->ledMovingButton);
}

void JogControlPanel::_motorStopped(unsigned short p_addr) {
    Q_UNUSED(p_addr);

    ui->ledMovingButton->setState(0);
    Themer::rePolish(ui->ledMovingButton);
}

void JogControlPanel::_prepJogInputs(unsigned short p_addr) {

    m_curAddr = p_addr;

        // update the jog speed limit and damping with the saved
        // values for the axis
    OMaxisOptions* opts = m_opts->getOptions(p_addr);

    unsigned int jog_limit = opts->jogLimit;
    unsigned int   max_jog = opts->maxSteps;

        // TODO: Add indicator for displaying max speed and damping

    float curMax  = m_jcm->stepsToJogSpeed(opts, jog_limit);
    float dispMax = m_jcm->stepsToJogSpeed(opts, max_jog);

    qDebug() << "JCP: Setting current speed max value to" << curMax;

    ui->jogSpeedSlider->setMinimum(1);
    ui->jogSpeedSlider->setMaximum(dispMax);
    ui->jogSpeedSlider->setValue(curMax);

    m_jcm->jogMaxSpeedChange(curMax);

    ui->jogDampSlider->setMinimum(1);
    ui->jogDampSlider->setMaximum(JCP_MAX_DAMP);
    ui->jogDampSlider->setValue((int) opts->jogDamp);

    m_jcm->jogResChange(1);

        // set home and end led states

        // pre-populate list with 'off' if not already populated
    if( ! m_ledStates.contains(p_addr) ) {
        QList<bool> list;
        list << false << false;

        m_ledStates.insert(p_addr, list);
    }

    ui->ledHomeButton->setState(m_ledStates.value(p_addr).at(0));
    ui->ledEndButton->setState(m_ledStates.value(p_addr).at(1));

    Themer::rePolish(ui->ledHomeButton);
    Themer::rePolish(ui->ledEndButton);

}

void JogControlPanel::_modeClicked() {

    if( m_curMode == false )
        ui->modeButton->setState(1);
    else
        ui->modeButton->setState(0);


    m_curMode = ! m_curMode;

    // force a re-draw of the button's style

    Themer::rePolish(ui->modeButton);
}

void JogControlPanel::_dialReleased() {
    if( m_curMode == true ) {
        ui->jogDial->setValue(0);
        m_jcm->speedChange(0);
    }
}

void JogControlPanel::_eStop() {
    ui->jogDial->setValue(0);
    emit emergencyStop();
}

void JogControlPanel::_themeChanged() {
    setStyleSheet(SingleThemer::getStyleSheet("jog"));
    Themer::rePolish(this);
}

void JogControlPanel::_jogMotorChangeAllowed(unsigned short p_addr) {
    _prepJogInputs(p_addr);
}

void JogControlPanel::_jogMotorChangeDenied(unsigned short p_oldAddr) {
    qDebug() << "JCP: Motor change was denied, re-setting devButtonList selection" << p_oldAddr;

    int row = m_ldModel->find(p_oldAddr);

    qDebug() << "JCP: Selecting Row" << row;

    QModelIndex tl = m_ldModel->index(row, 0, QModelIndex());
    QModelIndex tr = m_ldModel->index(row, 0, QModelIndex());
    QItemSelection sel(tl, tr);
    ui->devButtonList->selectionModel()->select(sel, QItemSelectionModel::SelectCurrent);
}

void JogControlPanel::_homeSet() {
    // record that home has been set for this axis
    // (we know that the list was populated by the
    // _prepJogInputs() method (or has no contents
    // if no device has been selected yet)

    if( m_ledStates.size() > 0 ) {
        QList<bool> list = m_ledStates.value(m_curAddr);
        list.replace(0, true);
        m_ledStates.insert(m_curAddr, list);
    }

    ui->ledHomeButton->setState(1);
    Themer::rePolish(ui->ledHomeButton);

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

        // record that end has been set for this axis
        // (we know that the list was populated by the
        // _prepJogInputs() method (or has no contents
        // if no device has been selected yet)

    if( m_ledStates.size() > 0 ) {
        QList<bool> list = m_ledStates.value(p_addr);
        list.replace(1, true);
        m_ledStates.insert(p_addr, list);
    }

    ui->ledEndButton->setState(1);
    Themer::rePolish(ui->ledEndButton);

    qDebug() << "JCP: End Setting Completed";
}

