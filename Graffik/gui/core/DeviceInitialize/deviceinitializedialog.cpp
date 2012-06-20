#include "deviceinitializedialog.h"
#include "ui_deviceinitializedialog.h"

DeviceInitializeDialog::DeviceInitializeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeviceInitializeDialog)
{
    ui->setupUi(this);
}

DeviceInitializeDialog::~DeviceInitializeDialog()
{
    delete ui;
}
