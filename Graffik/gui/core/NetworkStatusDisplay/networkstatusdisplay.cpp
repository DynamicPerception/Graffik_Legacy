#include "networkstatusdisplay.h"
#include "ui_networkstatusdisplay.h"

NetworkStatusDisplay::NetworkStatusDisplay(OMMoCoBus* c_bus, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NetworkStatusDisplay)
{
    ui->setupUi(this);

    m_bus = c_bus;
 //   m_busInfo = m_bus->getBusInfo();
    this->_updateDisplay();
}

NetworkStatusDisplay::~NetworkStatusDisplay()
{
    delete ui;
}

void NetworkStatusDisplay::_updateDisplay() {
 /*   ui->nameLabel->setText(m_busInfo->name);
    ui->portLabel->setText(m_busInfo->port);
    ui->commandLCD->display((int) m_busInfo->commands);
    ui->deviceLCD->display((int) m_busInfo->devices);
    ui->errorLCD->display((int) m_busInfo->errors);
    this->repaint();*/
}

