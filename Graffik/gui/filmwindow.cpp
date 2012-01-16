#include "filmwindow.h"
#include "ui_filmwindow.h"

filmWindow::filmWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::filmWindow)
{
    ui->setupUi(this);
}

filmWindow::~filmWindow()
{
    delete ui;
}
