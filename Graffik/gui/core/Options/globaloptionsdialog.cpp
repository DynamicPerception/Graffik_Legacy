#include "globaloptionsdialog.h"
#include "ui_globaloptionsdialog.h"

GlobalOptionsDialog::GlobalOptionsDialog(GlobalOptions *c_opts, QWidget *parent) : QDialog(parent), ui(new Ui::GlobalOptionsDialog) {
    ui->setupUi(this);

    m_opts = c_opts;

    _setupInputs();
}

GlobalOptionsDialog::~GlobalOptionsDialog()
{
    delete ui;
}

void GlobalOptionsDialog::accept() {
    bool stop = ui->stopCheckBox->isChecked();
    int disp  = ui->displayCombo->currentIndex();

    m_opts->stopOnErr(stop);

    if( disp == 0 )
        m_opts->display(Options::Imperial);
    else if( disp == 1 )
        m_opts->display(Options::Metric);
    else if( disp == 2 )
        m_opts->display(Options::Steps);

    QDialog::accept();
}

void GlobalOptionsDialog::_setupInputs() {

    ui->stopCheckBox->setChecked(m_opts->stopOnErr());
    int dispTyp = m_opts->display();

    ui->displayCombo->addItem(GOD_IMP_LABEL);
    ui->displayCombo->addItem(GOD_MET_LABEL);
    ui->displayCombo->addItem(GOD_STP_LABEL);

    if( dispTyp == Options::Imperial )
        ui->displayCombo->setCurrentIndex(0);
    else if( dispTyp == Options::Metric )
        ui->displayCombo->setCurrentIndex(1);
    else if( dispTyp == Options::Steps)
        ui->displayCombo->setCurrentIndex(2);

}
