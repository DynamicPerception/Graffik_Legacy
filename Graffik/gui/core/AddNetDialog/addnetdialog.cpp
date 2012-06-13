#include <QList>
#include <QVariant>

#include <QDebug>

#include "addnetdialog.h"
#include "ui_addnetdialog.h"

#include "qextserialenumerator.h"

#include "Core/ErrorDialog/errordialog.h"

addNetDialog::addNetDialog(OMNetwork *c_net, QWidget *c_parent) :
    QDialog(c_parent),
    ui(new Ui::addNetDialog)
{
    ui->setupUi(this);

    _net = c_net;

    _thsColor = QColor("blue");

    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    _setNetBackground(_thsColor);

    updateSerialPorts();
}

addNetDialog::~addNetDialog()
{
    delete ui;
}

void addNetDialog::_setNetBackground(QColor p_color) {
    // unfortunately, we cannot use a palette on windows, so
    // we need to use a stylesheet instead
    QString myStyle = "background-color: rgb(%1,%2,%3);";
    ui->colorSetButton->setStyleSheet(myStyle.arg(_thsColor.red()).arg(_thsColor.green()).arg(_thsColor.blue()));
}

void addNetDialog::on_colorSetButton_clicked() {
    _thsColor = QColorDialog::getColor(_thsColor, this, "Bus Color");
    _setNetBackground(_thsColor);
}

void addNetDialog::updateSerialPorts() {
    QList<QextPortInfo> ports = QextSerialEnumerator::getPorts();
    QList<QString> usedPorts = _net->getBuses();

        // clear out the combo box
    while( ui->portCombo->count() > 0 ) {
        ui->portCombo->removeItem( ui->portCombo->count() - 1 );
    }

    foreach( QextPortInfo port, ports ) {
        // do not add port to select if it is already in use
        if( usedPorts.contains(port.portName) ) {
            qDebug() << "Already have used port " << port.portName;
            continue;
        }

        ui->portCombo->addItem(port.portName, QVariant(port.friendName));
    }

    ui->portHintLabel->setText(ports[0].friendName);
}

void addNetDialog::on_rescanButton_clicked() {
    this->updateSerialPorts();
}

void addNetDialog::on_portCombo_currentIndexChanged(int p_idx) {
    ui->portHintLabel->setText(ui->portCombo->itemData(p_idx).toString());

}


void addNetDialog::accept() {
    QString port = ui->portCombo->itemText(ui->portCombo->currentIndex());
    QString name = ui->netName->text();

    bool ok = true;
    QString errString = "";

    if( name.isEmpty() ) {
        errString = "You must enter a name for this bus.";
        ok = false;
    }

    if( ok == true ) {
        try {
            _net->addBus(port, name);
        }
        catch (int e) {
            ok = false;
            if( e == OM_NET_DUPE ) {
                errString = "The communication port selected is already in-use";
            }
            else {
                errString = "An unknown error occured, please try again. Error Code: " + e;
            }
        }
    }


    if( ok == false ) {
        // throw out error dialog
        ErrorDialog er(this);
        er.setError(errString);
        er.exec();
    }
    else {
            // set color
        _net->busColor(port, _thsColor);
        setResult(QDialog::Accepted);
        QDialog::accept();
    }

}
