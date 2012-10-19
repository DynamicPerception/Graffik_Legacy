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

#include "slimwindow.h"
#include "ui_slimwindow.h"


#include <QDebug>
#include <QtEndian>

#include "core/Dialogs/errordialog.h"


SlimWindow::SlimWindow(OMNetwork* net, CommandHistoryModel* hist, SlimCommandParser* parse, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SlimWindow)
{

    ui->setupUi(this);

    _parser = parse;
    _net = net;
    _cmdHist = hist;

    ui->historyTableView->setModel(_cmdHist);

    QObject::connect(_cmdHist, SIGNAL(commandResults(slimHistoryEntry)), this, SLOT(onCmdResult(slimHistoryEntry)));


}

SlimWindow::~SlimWindow()
{

    delete ui;
}


void SlimWindow::onCmdEntry() {

     QString inputStr = ui->commandInputLine->displayText();

     ui->commandInputLine->clear();

     slimCommand newCmd;

     try {
         newCmd = _parser->parse(inputStr);
     }
     catch (int e) {
         qDebug() << "SlimCmd: Received exception: " << e;

       //      throw out error dialog
         ErrorDialog er(this);

         QString erMsg = "";

         if( e == SLIM_ERR_ARG )
             erMsg = "An invalid argument was provided for the command";
         else if( e == SLIM_ERR_ARGS )
             erMsg = "Invalid number of arguments provided to command";
         else if( e == SLIM_ERR_NOCMD )
             erMsg = "Unknown command";
         else if( e == SLIM_ERR_NODEV )
             erMsg = "The specified device was not found";
         else if( e == SLIM_ERR_EMPTY )
             erMsg = "No command was input";
         else if( e == SLIM_ERR_NOBUS )
             erMsg = "No bus was found, you must add a bus first";
         else
             erMsg = "An Unknown Error Occurred: " + QString::number(e);

         er.setError(erMsg);
         er.exec();

         return;
     }

     //    successfully entered command, add to history

     _cmdHist->insertRow(newCmd);
     ui->historyTableView->scrollToBottom();

}

void SlimWindow::onCmdResult(slimHistoryEntry p_cmd) {
    QString com = p_cmd.command;

    OMCommandBuffer* buf = p_cmd.cmdObject.buf;
    unsigned int resSize = buf->resultSize();

    QString resStr;

    if( resSize > 0 ) {

        char* res = new char[resSize];
        buf->result(res, resSize);


        int resType = buf->resultType();

            // convert response data type

        if( resType == R_BYTE ) {
            resStr.setNum(res[0]);
        }
        else if( resType == R_UINT) {
            qint16 foo = qFromBigEndian<qint16>((uchar*) res);
            resStr.setNum((unsigned short) foo);
        }
        else if( resType == R_INT) {
            qint16 foo = qFromBigEndian<qint16>((uchar*) res);
            resStr.setNum((short) foo);

        }
        else if( resType == R_ULONG ) {
            qint32 foo = qFromBigEndian<qint32>((uchar*)res);
            resStr.setNum((unsigned long) foo);
        }
        else if( resType == R_LONG ) {
            qint32 foo = qFromBigEndian<qint32>((uchar*)res);
            resStr.setNum((long) foo);
        }
        else if( resType == R_FLOAT ) {
            qint32 foo = qFromBigEndian<qint32>((uchar*)res);
            resStr.setNum((float) foo);
        }
        else if( resType == R_STRING) {
            resStr = QString::fromAscii(res, resSize);
        }

        delete res;

    }

    ui->commandResults->append(com);
    ui->commandResults->append("    " + resStr);


    qDebug() << "SW: Response Data from command " << com;

}



void SlimWindow::registerNewDevice(OMbusInfo *p_bus, OMdeviceInfo *p_dev) {
    if( _parser != 0 ) {
        qDebug() << "SW: Add to Slim Parser";
            // register device with slim command parser
        SlimScriptDevice* sdev = dynamic_cast<SlimScriptDevice*>(p_dev->device);
        if( sdev != 0 ) {
                // do not attempt to add the device if it is not derived
                // from SlimScriptDevice
            _parser->registerDevice(sdev, p_bus->bus->port(), p_dev->device->address());
            _parser->addAlias(p_dev->name, p_bus->bus->port(), p_dev->device->address());
        }
    }

}

void SlimWindow::removeDevice(OMbusInfo *p_bus, unsigned short p_addr) {
    QString port = p_bus->bus->port();

    if( _parser != 0 ) {
        qDebug() << "SW: Remove from Slim Parser" << p_addr;
        _parser->removeDevice(port, p_addr);
    }
}


void SlimWindow::on_loadButton_clicked() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Script"), "", tr("Slim Scripts (*.slim)"));
    qDebug() << "SW: SlimOpen Got File: " << fileName;
    SlimFileHandler::readFile(fileName, _parser, _cmdHist, true);
}

void SlimWindow::on_saveButton_clicked() {
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Script"), "", tr("Slim Script (*.slim)"));
    qDebug() << "SW: SlimSave Got File: " << fileName;
    SlimFileHandler::writeFile(fileName, _cmdHist, true);
}

