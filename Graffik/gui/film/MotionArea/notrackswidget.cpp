#include "notrackswidget.h"
#include "ui_notrackswidget.h"

NoTracksWidget::NoTracksWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NoTracksWidget)
{
    ui->setupUi(this);
}

NoTracksWidget::~NoTracksWidget()
{
    delete ui;
}
