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

#include "helpwindow.h"
#include "ui_helpwindow.h"

#include <QDebug>

HelpWindow::HelpWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HelpWindow)
{
    m_help = new QHelpEngine(QApplication::applicationDirPath() + "/docs/docs.qhc", this);
    m_browse = new HelpBrowser(m_help);

    ui->setupUi(this);

    m_help->setupData();

    if( m_help->error().length() > 0 ) {
        qDebug() << "Error: " << m_help->error();
    }

    QHelpContentWidget* content_w = m_help->contentWidget();

    ui->splitter->addWidget((QWidget*)content_w);
    ui->splitter->addWidget(m_browse);

    QObject::connect((QObject*)content_w, SIGNAL(linkActivated(const QUrl&)), m_browse, SLOT(setSource(const QUrl&)));


}

HelpWindow::~HelpWindow()
{
    delete m_browse;
    delete m_help;
    delete ui;

}

void HelpWindow::on_closeButton_clicked() {
    this->close();
}

