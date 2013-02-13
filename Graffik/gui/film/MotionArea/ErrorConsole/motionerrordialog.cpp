#include "motionerrordialog.h"
#include "ui_motionerrordialog.h"

MotionErrorDialog::MotionErrorDialog(QList<QString> c_errs, QWidget *parent) : QDialog(parent), ui(new Ui::MotionErrorDialog) {
    ui->setupUi(this);

    m_areaLayout = new QVBoxLayout;
    m_areaLayout->setContentsMargins(0, 0, 0, 0);
    m_areaLayout->setSpacing(0);

    ui->scrollAreaWidgetContents->setLayout(m_areaLayout);


    foreach(QString err, c_errs) {
        MotionErrorElement* errElem = new MotionErrorElement(err, this);
        m_areaLayout->addWidget(errElem);
        errElem->show();
    }


    m_areaLayout->addStretch(0);
    setStyleSheet(SingleThemer::getStyleSheet("motion_error"));

}

MotionErrorDialog::~MotionErrorDialog() {
    delete m_areaLayout;
    delete ui;
}
