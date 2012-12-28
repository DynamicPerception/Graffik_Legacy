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

#include "commandhistorymodel.h"
#include <QDebug>



CommandHistoryModel::CommandHistoryModel(OMNetwork *c_net, QObject *parent) :
    QAbstractTableModel(parent)
{
    m_net = c_net;

    m_resQueued = new QString("Queued");
    m_resBcast  = new QString("Broadcast");

    m_createdStrings.append(m_resQueued);
    m_createdStrings.append(m_resBcast);

    m_queLed = new QPixmap;
    m_errLed = new QPixmap;
    m_okLed  = new QPixmap;
    m_bcLed  = new QPixmap;

    qRegisterMetaType<slimCommand>("slimCommand");

        // we want the results of each command
    QObject::connect(m_net, SIGNAL(complete(int, OMCommandBuffer*)), this, SLOT(_commandCompleted(int, OMCommandBuffer*)), Qt::QueuedConnection);

}

CommandHistoryModel::~CommandHistoryModel() {

    foreach(QString* str, m_createdStrings)
        delete str;

    delete m_queLed;
    delete m_errLed;
    delete m_okLed;
    delete m_bcLed;
}

int CommandHistoryModel::rowCount(const QModelIndex &) const {
        return _cmdHist.size();
 }

 int CommandHistoryModel::columnCount(const QModelIndex &) const {
     return 5; // 5 columns in slimCommand for display
 }

 QVariant CommandHistoryModel::headerData(int section, Qt::Orientation orientation, int role) const {

     if( role == Qt::DisplayRole ) {
         if( orientation == Qt::Horizontal ) {
             switch(section) {
                case 0:
                 return QVariant("Device");
                case 1:
                 return QVariant("Command");
                case 2:
                 return QVariant("Bus");
                case 3:
                 return QVariant("Stat");
                case 4:
                 return QVariant("Response");
                default:
                 return QVariant();
             }
         }
         else if( orientation == Qt::Vertical ) {
             return QVariant(section);
         }
     }

     return QVariant();
 }

 // overridden data selector

 QVariant CommandHistoryModel::data(const QModelIndex &index, int role) const {
  //   qDebug() << "Asked for" << index.row() << " and " << index.column();

     if( index.row() > this->rowCount() )
         return QVariant();

     if( index.column() > this->columnCount() - 1)
         return QVariant();


     if (role == Qt::DisplayRole) {

        if( index.column() == 2 )             // get bus name
            return QVariant(_cmdHist.at(index.row()).bus);
        else if( index.column() == 0 )        // get device name
            return QVariant(_cmdHist.at(index.row()).deviceName);
        else if( index.column() == 1 )        // command issued
            return QVariant(_cmdHist.at(index.row()).command);
        else if( index.column() == 4 )        // result text
            return QVariant(*_cmdHist.at(index.row()).commandResult);

     }
     else if( role == Qt::TextAlignmentRole ) {
         if( index.column() == 0 )
            return Qt::AlignRight;
         else if(index.column() == 3 || index.column() == 2 )
             return Qt::AlignCenter;
         else
             return Qt::AlignLeft;

     }
     else if( role == Qt::DecorationRole ) {
         if( index.column() == 3 ) {
                 // result/status light

                    // broadcast always succeeds...
            if( _cmdHist.at(index.row()).broadcast ) {
                return QVariant(QIcon(*m_bcLed));

             }
             else {
                 if( _cmdHist.at(index.row()).status == OMC_QUEUED )
                     return QVariant(QIcon(*m_queLed));
                 else if( _cmdHist.at(index.row()).status == OMC_FAILURE )
                     return QVariant(QIcon(*m_errLed));
                 else if( _cmdHist.at(index.row()).status == OMC_SUCCESS )
                     return QVariant(QIcon(*m_okLed));
                 else
                     return QVariant(QIcon(*m_errLed));
             }

         }
     }

     return QVariant();
 }

 bool CommandHistoryModel::insertRow(slimCommand p_com) {

        // populate rows in the model with interpreted data
     qDebug() << "SCHM: Inserting command history row for command" << p_com.id;

     beginInsertRows(QModelIndex(), this->rowCount(), this->rowCount());

     QString bName;
     QString dName;
     QString* res;
     unsigned short dAddr;

        // use default strings and values for broadcast commands
     if(p_com.broadcast) {
         bName = "broadcast";
         dName = "broadcast";
         res   = m_resBcast;
         dAddr = 0;
     }
     else {
         bName = m_net->busInfo(p_com.network)->name;
         dName = m_net->deviceInfo(p_com.network, p_com.address)->name;
         dAddr = p_com.address;
         res   = m_resQueued;
     }

     QString cmd = p_com.command + " " + p_com.arguments.join(" ");


     slimHistoryEntry ent(p_com, bName, dName, dAddr, OMC_QUEUED, cmd, p_com.broadcast, res);

     _cmdHist.append(ent);



     qDebug() << "SCHM: Model: inserted new row for command id " << p_com.id;

        // register with the command manager, that we want to hold onto this
        // command buffer

     m_net->getManager()->hold(p_com.id);

     endInsertRows();

     int newRow = this->rowCount();

        // keep track of which row has our command,
        // so that we don't have to iterate through the
        // vector when the command is completed

     m_cmdLoc[p_com.id] = newRow - 1;


     return(true);
 }

 void CommandHistoryModel::_commandCompleted(int p_id, OMCommandBuffer* p_buf) {


        // strange, we have no record of that command! (Must've come from someone else)
     if( ! m_cmdLoc.contains(p_id) )
         return;

     qDebug() << "SCHM: Received complete signal for command we issued";

     // considering that we would've gotten this command with no actual OMCommandBuffer pointer, let's resolve
     // that

     int thsRow = m_cmdLoc[p_id];

     _cmdHist.at(thsRow).cmdObject.buf = p_buf;

     _cmdHist.at(thsRow).status = p_buf->status();

     qDebug() << "SCHM: Command Result Size: " << p_buf->resultSize();

        // display resulting response
     _processResults(thsRow, p_buf);

     emit dataChanged(createIndex(thsRow, 0), createIndex(thsRow, 5));

 }

 slimHistoryEntry CommandHistoryModel::getCommand(int p_row) {
     return _cmdHist.at(p_row);
 }

 void CommandHistoryModel::_processResults(int p_row, OMCommandBuffer* p_buf) {



     unsigned int resSize = p_buf->resultSize();

     QString* resStr = new QString();

     if( p_buf->status() == OMC_FAILURE ) {
         *resStr = QString("Failed");
     }
     else if( p_buf->status() == OMC_TIMEOUT ) {
         *resStr = QString("Timed Out");
     }
     else if( resSize > 0 ) {

         char* res = new char[resSize];
         p_buf->result(res, resSize);


         int resType = p_buf->resultType();

             // convert response data type

         if( resType == R_BYTE ) {
             resStr->setNum(res[0]);
         }
         else if( resType == R_UINT) {
             qint16 foo = qFromBigEndian<qint16>((uchar*) res);
             resStr->setNum((unsigned short) foo);
         }
         else if( resType == R_INT) {
             qint16 foo = qFromBigEndian<qint16>((uchar*) res);
             resStr->setNum((short) foo);

         }
         else if( resType == R_ULONG ) {
             qint32 foo = qFromBigEndian<qint32>((uchar*)res);
             resStr->setNum((unsigned long) foo);
         }
         else if( resType == R_LONG ) {
             qint32 foo = qFromBigEndian<qint32>((uchar*)res);
             resStr->setNum((long) foo);
         }
         else if( resType == R_FLOAT ) {
             qint32 foo = qFromBigEndian<qint32>((uchar*)res);
             resStr->setNum((float) foo);
         }
         else if( resType == R_STRING) {
             *resStr = QString::fromAscii(res, resSize);
         }

         delete res;

     }
     else {
         *resStr = QString("OK");
     }

     _cmdHist.at(p_row).commandResult = resStr;

     m_createdStrings.append(resStr);

     qDebug() << "SCHM: Response Data from command " << *resStr;

 }

 QPixmap CommandHistoryModel::errLED() {
     return *m_errLed;
 }

 QPixmap CommandHistoryModel::okLED() {
     return *m_okLed;
 }

 QPixmap CommandHistoryModel::bcastLED() {
     return *m_bcLed;
 }

 QPixmap CommandHistoryModel::queueLED() {
     return *m_queLed;
 }

 void CommandHistoryModel::setErrLED(QPixmap p_pix) {
     qDebug() << "SCHM: Got Err LED";
     *m_errLed = p_pix;
 }

 void CommandHistoryModel::setOkLED(QPixmap p_pix) {
     *m_okLed = p_pix;
 }

 void CommandHistoryModel::setBcastLED(QPixmap p_pix) {
     *m_bcLed = p_pix;
 }

 void CommandHistoryModel::setQueueLED(QPixmap p_pix) {
     *m_queLed = p_pix;
 }
