#include "deletedevicedialog.h"
#include "ui_deletedevicedialog.h"

DeleteDeviceDialog::DeleteDeviceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeleteDeviceDialog)
{
    ui->setupUi(this);
}

DeleteDeviceDialog::~DeleteDeviceDialog()
{
    delete ui;
}
