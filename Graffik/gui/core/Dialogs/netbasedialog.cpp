#include "netbasedialog.h"
#include "ui_netbasedialog.h"



NetBaseDialog::NetBaseDialog(OMNetwork *c_net, NetworkModel *c_model, AxisOptions *c_opts, QWidget *parent) : QDialog(parent), ui(new Ui::NetBaseDialog) {
    ui->setupUi(this);

    m_net = c_net;
    m_opts = c_opts;
    m_model = c_model;
    m_scan = 0;
}

NetBaseDialog::~NetBaseDialog()
{
    delete ui;

    if( m_scan != 0)
        delete m_scan;

}


void NetBaseDialog::on_scanButton_clicked() {
    if( m_scan != 0)
        delete m_scan;

    m_scan = new DeviceScanner(m_net);
}


void NetBaseDialog::on_initButton_clicked() {
    if( m_scan != 0)
        delete m_scan;

    m_scan = new DeviceScanner(m_net, 2);
}

void NetBaseDialog::on_manageButton_clicked() {
    NetworkManager netMan(m_model, m_opts, this);
    netMan.exec();
}
