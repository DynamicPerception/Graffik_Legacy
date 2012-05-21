#include "slimwindow.h"
#include "ui_slimwindow.h"


#include <QDebug>
#include <QtEndian>

#include "Core/ErrorDialog/errordialog.h"

SlimWindow::SlimWindow(OMNetwork* net, SlimCommandParser* parse, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SlimWindow)
{

    ui->setupUi(this);

    _parser = parse;
    _net = net;
    _cmdHist = new CommandHistoryModel(_net, this);

    ui->historyTableView->setModel(_cmdHist);

//    qRegisterMetaType<slimCommand>("slimCommand");

    QObject::connect(_cmdHist, SIGNAL(commandResults(slimCommand)), this, SLOT(onCmdResult(slimCommand)), Qt::QueuedConnection);

        // add grid layout to "Network" toolbox (index 1 - TODO: not use fixed index)
   // _netState = new NetworkStatusGrid(ui->RightToolBox->widget(1));

    // ui->verticalLayout->setMenuBar(ui->menuBar);
}

SlimWindow::~SlimWindow()
{
    delete ui;
    delete _cmdHist;
}


void SlimWindow::onCmdEntry() {

     qDebug() << "Enter pressed!";

     QString inputStr = ui->commandInputLine->displayText();

     ui->commandInputLine->clear();

     slimCommand newCmd;

     try {
         newCmd = _parser->parse(inputStr);
     }
     catch (int e) {
         qDebug() << "Received exception: " << e;

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

         er.setError(erMsg);
         er.exec();

         return;
     }

     //    successfully entered command, add to history

     _cmdHist->insertRow(newCmd);
     ui->historyTableView->scrollToBottom();

}

void SlimWindow::onCmdResult(slimCommand p_cmd) {
    QString com = p_cmd.command + " " + p_cmd.arguments.join(" ") + ":";

    unsigned int resSize = p_cmd.buf->resultSize();

    QString resStr;

    if( resSize > 0 ) {

        char* res = new char[resSize];
        p_cmd.buf->result(res, resSize);


        int resType = p_cmd.buf->resultType();

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


    qDebug() << "Response Data from command " << com;

}

void SlimWindow::onDialActivate() {
    qDebug() << "Dial-a-speed";
    DialSpeed * _dialSpd = new DialSpeed(this);

    _dialSpd->show();
}

void SlimWindow::registerNewDevice(OMbusInfo *p_bus, OMdeviceInfo *p_dev) {
    if( _parser != 0 ) {
        qDebug() << "Add to Slim Parser";
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

/*void SlimWindow::onSlide(int value) {
    qDebug() << value;

   unsigned short dispSpeed = 1.0;
   QString dir = "L";
   boolean adir = true;
   static DWORD ticks = 0;

   if( value < 500 ) {
       dispSpeed = (unsigned short) (((500.0 - (double) value) / 500.00) * 1000.0);
       dir = "L";
       adir = false;
   }
   else if(value > 500) {
       dispSpeed = (unsigned short) ((((double) value - 500.0) / 500.0) * 1000.0);
       dir = "R";
   }
   else {
       dispSpeed = 0.0;
       dir = "";
   }

   qDebug() << "Sending value: " << (unsigned short) dispSpeed;
   qDebug() << "Tick count:" << (DWORD) ticks;

   if( _axis != 0 && (ticks == 0 || GetTickCount() - ticks > 500) ) {
       try {
           int cmdId = _axis->direction(adir);
           qDebug() << "DIRID: " << cmdId;
           cmdId = _axis->speed(dispSpeed);
           qDebug() << "SPDID: " << cmdId;
           ticks = GetTickCount();
       }
       catch( int e ) {
           qDebug()<< "Caught Exception: " << e;
       }
   }
   emit(updateMotorDisp((int) dispSpeed));
   emit(updateMotorDir(dir));
}

void SlimWindow::on_serialPortButton_clicked() {
    QString thsText = ui->serInput->text();


    try {
            // convert to char*
        QByteArray ba = thsText.toLocal8Bit();
        char* thsPort = ba.data();
        _ctrl->setPort(thsPort);

        _ctrl->connect();

        int cmdId = _axis->stopMotor();

        cmdId = _axis->sleep(true);

        cmdId = _axis->speed(1);

        cmdId = _axis->move(true, 0);


    }
    catch (int e) {
        qDebug()<< "Caught Exception: " << e;
    }


}
*/

