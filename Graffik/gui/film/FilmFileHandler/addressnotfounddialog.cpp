#include "addressnotfounddialog.h"
#include "ui_addressnotfounddialog.h"


#include <QDebug>

AddressNotFoundDialog::AddressNotFoundDialog(unsigned short c_addr, QHash<unsigned short, QString> c_list, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddressNotFoundDialog)
{
    ui->setupUi(this);
    m_replace = c_list;

    setWindowModality(Qt::WindowModal);

    foreach(QString name, m_replace.values()) {
        ui->devCombo->addItem(name);
    }

    ui->addressLabel->setText(QString::number(c_addr));

}

AddressNotFoundDialog::~AddressNotFoundDialog()
{
    delete ui;
}

void AddressNotFoundDialog::accept() {

    QString selectedName = ui->devCombo->currentText();
    int selAddr = 1;

    qDebug() << "ANFD: Selected new device: " << selectedName;

    foreach( unsigned short addr, m_replace.keys() ) {
        if( m_replace.value(addr) == selectedName )
            selAddr = addr;
    }

    qDebug() << "ANFD: New device address:" << selAddr;

   // setResult(selAddr);
   // QDialog::accept();
    QDialog::done(selAddr);

    qDebug() << "ANFD: Done done";

}
