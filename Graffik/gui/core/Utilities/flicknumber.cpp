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

    unsigned int rowCnt = ui->listWidget->count() - 1;

    if( rowCnt == p_count )
        return;

    for( int i = rowCnt; i >= 0; i--) {
        QListWidgetItem* item = ui->listWidget->takeItem(i);
        delete item;
    }

    for( unsigned int i = 0; i <= p_count; i++ ) {

        QString first;
        QString second;

        if(i < 10 ) {
            first = "0";
            second = QString::number(i);
        }
        else {
            first = QString::number(i / 10);
            second = QString::number( i - ((i / 10) * 10));
        }

        QListWidgetItem* widg = new QListWidgetItem(first + "  " + second);
        widg->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->listWidget->addItem(widg);

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
   // _setLabels(val);
}

void FlickNumber::setValue(unsigned int p_val) {
    if( (unsigned int) ui->listWidget->count() < p_val )
        return;

    ui->listWidget->setCurrentRow(p_val);
   // _setLabels(p_val);
}

unsigned int FlickNumber::value() {

    int val = ui->listWidget->currentRow();
    val = val >= 0 ? val : 0;
    return val;

}


void FlickNumber::resizeEvent(QResizeEvent* p_evt) {

    QSize sz = p_evt->size();

    for( int i = (ui->listWidget->count() - 1); i >= 0; i--) {
        QListWidgetItem* item = ui->listWidget->item(i);
        item->setSizeHint(sz);
    }
}

void FlickNumber::setEnabled(bool p_en) {
    ui->listWidget->setEnabled(p_en);

    if( p_en == true ) {
        m_fcp->activateOn(ui->listWidget);
        connect(m_fcp, SIGNAL(flickSettled()), this, SLOT(flickSettled()));
    }
    else {
        m_fcp->deactivateFrom(ui->listWidget);
        disconnect(m_fcp, SIGNAL(flickSettled()), this, SLOT(flickSettled()));
    }
}
