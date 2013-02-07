#include "motionerrorelement.h"
#include "ui_motionerrorelement.h"

MotionErrorElement::MotionErrorElement(QString c_err, QWidget *parent) : QWidget(parent), ui(new Ui::MotionErrorElement) {
    ui->setupUi(this);

    ui->errorText->setText(c_err);

    setStyleSheet(SingleThemer::getStyleSheet("motion_error_element"));
}

MotionErrorElement::~MotionErrorElement() {
    delete ui;
}
