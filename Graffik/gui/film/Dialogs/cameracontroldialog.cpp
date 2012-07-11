#include "cameracontroldialog.h"
#include "ui_cameracontroldialog.h"

CameraControlDialog::CameraControlDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CameraControlDialog)
{
    ui->setupUi(this);
}

CameraControlDialog::~CameraControlDialog()
{
    delete ui;
}
