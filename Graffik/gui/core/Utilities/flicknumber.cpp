/*

  Graffik Motion Control Application

  Copyright (c) 2011-2012 Dynamic Perception

 This file is part of Graffik.

    Graffik is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Graffik is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Graffik.  If not, see <http://www.gnu.org/licenses/>.

    */

#include "flicknumber.h"
#include "ui_flicknumber.h"


#include <QDebug>
#include <QScrollBar>

FlickNumber::FlickNumber(unsigned int c_count, QWidget *parent) : QWidget(parent), ui(new Ui::FlickNumber) {
    ui->setupUi(this);

    setCount(c_count);
    _setFlick();
}

FlickNumber::FlickNumber(QWidget *parent) : QWidget(parent), ui(new Ui::FlickNumber) {
    ui->setupUi(this);
    _setFlick();
}

FlickNumber::~FlickNumber()
{

        // delete all items added to the listwidget
    for( int i = (ui->listWidget->count() - 1); i >= 0; i--) {
        QListWidgetItem* item = ui->listWidget->takeItem(i);
        delete item;
    }

    delete ui;
    delete m_fcp;

}

void FlickNumber::setCount(unsigned int p_count) {

    int rowCnt = ui->listWidget->count() - 1;

    if( rowCnt == p_count )
        return;

    for( int i = rowCnt; i >= 0; i--) {
        QListWidgetItem* item = ui->listWidget->takeItem(i);
        delete item;

    }

    for( unsigned int i = 0; i <= p_count; i++ ) {
          ui->listWidget->addItem(QString::number(i));
          //      QImage img = _genIconImage(i);
         //       QListWidgetItem* itm = new QListWidgetItem();
         //       itm->setIcon(QIcon(QPixmap::fromImage(img)));
         //       ui->listWidget->addItem(itm);

    }

}

void FlickNumber::_setFlick() {
    m_fcp = new FlickCharm;
    m_fcp->activateOn(ui->listWidget);

    connect(m_fcp, SIGNAL(flickSettled()), this, SLOT(flickSettled()));
}

void FlickNumber::flickSettled() {

    unsigned int val = ui->listWidget->verticalScrollBar()->sliderPosition();

    ui->listWidget->setCurrentRow(val);
    emit valueChanged(val);
}

void FlickNumber::setValue(unsigned int p_val) {
    if( (unsigned int) ui->listWidget->count() < p_val )
        return;

    ui->listWidget->setCurrentRow(p_val);
}

unsigned int FlickNumber::value() {

    int val = ui->listWidget->currentRow();
    val = val >= 0 ? val : 0;
    return val;

}

QImage FlickNumber::_genIconImage(unsigned int p_val) {

    QString fileBase = ":/numbers/number-%1.png";

    if( p_val < 10 ) {
        fileBase = fileBase.arg(QString::number(p_val), 0, QLatin1Char( ' ' ));
        qDebug() << "FlN: _gII: File" << fileBase;
        QImage img(fileBase);
        return img;
    }
    else {
        QImage img;
        return img;
    }


}
