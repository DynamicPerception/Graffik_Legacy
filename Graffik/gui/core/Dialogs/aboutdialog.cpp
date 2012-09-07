#include "aboutdialog.h"
#include "ui_aboutdialog.h"

#ifndef GRAFFIK_VERSION
#define GRAFFIK_VERSION 0.0
#endif

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    ui->versionLabel->setText(QString::number(GRAFFIK_VERSION));
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
