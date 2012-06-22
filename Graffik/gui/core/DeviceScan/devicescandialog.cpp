#include "devicescandialog.h"
#include "ui_devicescandialog.h"

DeviceScanDialog::DeviceScanDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeviceScanDialog)
{
    ui->setupUi(this);
    ui->doneButton->setText("Done");
}

DeviceScanDialog::~DeviceScanDialog()
{
    delete ui;
}

void DeviceScanDialog::enableConfirm(bool p_enable) {
    if( p_enable ) {
        ui->doneButton->setEnabled(true);
        ui->cancelButton->setDefault(false);
        ui->doneButton->setDefault(true);
    }
    else {
        ui->doneButton->setEnabled(false);
    }
}

void DeviceScanDialog::addNote(QString p_note) {
    ui->statusText->append(p_note);
}


void DeviceScanDialog::totalNodes(int p_nodes) {
    ui->progressBar->setMaximum(p_nodes);
    ui->progressBar->setMinimum(0);
}

void DeviceScanDialog::scannedNodes(int p_nodes) {
    ui->progressBar->setValue(p_nodes);
}


void DeviceScanDialog::nodesFound(bool p_found) {
    if( p_found == true )
        ui->doneButton->setText("Next");
    else
        ui->doneButton->setText("Done");
}

void DeviceScanDialog::on_cancelButton_clicked() {
    reject();
}

void DeviceScanDialog::on_doneButton_clicked() {
    accept();
}
