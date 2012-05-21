#include "omaxisfilmoptionsdialog.h"
#include "ui_omaxisfilmoptionsdialog.h"

OMAxisFilmOptionsDialog::OMAxisFilmOptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OMAxisFilmOptionsDialog)
{
    ui->setupUi(this);
}

OMAxisFilmOptionsDialog::~OMAxisFilmOptionsDialog()
{
    delete ui;
}
