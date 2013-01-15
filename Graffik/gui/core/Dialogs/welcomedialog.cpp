#include "welcomedialog.h"
#include "ui_welcomedialog.h"

WelcomeDialog::WelcomeDialog(QWidget *parent) : QDialog(parent), ui(new Ui::WelcomeDialog) {
    ui->setupUi(this);

    setStyleSheet(SingleThemer::getStyleSheet("welcome"));
}

WelcomeDialog::~WelcomeDialog() {
    delete ui;
}

void WelcomeDialog::on_addButton_clicked() {
    emit addBus();
    done(QDialog::Accepted);
}

void WelcomeDialog::on_cancelButton_clicked() {
    done(QDialog::Rejected);
}
