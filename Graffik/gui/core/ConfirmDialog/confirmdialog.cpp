#include "confirmdialog.h"
#include "ui_confirmdialog.h"

ConfirmDialog::ConfirmDialog(QString c_text, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfirmDialog)
{
    ui->setupUi(this);
    ui->contentLabel->setText(c_text);
}

ConfirmDialog::~ConfirmDialog()
{
    delete ui;
}
