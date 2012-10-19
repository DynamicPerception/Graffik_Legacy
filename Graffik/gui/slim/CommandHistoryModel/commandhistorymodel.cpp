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
#include <QBrush>
#include <QMetaType>
#include <QIcon>


CommandHistoryModel::CommandHistoryModel(OMNetwork *c_net, QObject *parent) :
    QAbstractTableModel(parent)
{
    m_net = c_net;

    qRegisterMetaType<slimCommand>("slimCommand");

        // we want the results of each command
    QObject::connect(m_net, SIGNAL(complete(int, OMCommandBuffer*)), this, SLOT(_commandCompleted(int, OMCommandBuffer*)), Qt::QueuedConnection);

}

CommandHistoryModel::~CommandHistoryModel() {
}

int CommandHistoryModel::rowCount(const QModelIndex &) const {
        return _cmdHist.size();
 }

 int CommandHistoryModel::columnCount(const QModelIndex &) const {
     return 4; // 4 columns in slimCommand for display
 }

 QVariant CommandHistoryModel::headerData(int section, Qt::Orientation orientation, int role) const {

     if( role == Qt::DisplayRole ) {
         if( orientation == Qt::Horizontal ) {
             switch(section) {
                case 0:
                 return QVariant("Bus");
                case 1:
                 return QVariant("Stat");
                case 2:
                 return QVariant("Device");
                case 3:
                 return QVariant("Command");
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

        if( index.column() == 0 ) {
            // get bus name
            return QVariant(_cmdHist.at(index.row()).bus);
        }

        else if( index.column() == 2 ) {
            // get device name
            return QVariant(_cmdHist.at(index.row()).deviceName);
        }
        else if( index.column() == 3 ) {
            // command issued
            return QVariant(_cmdHist.at(index.row()).command);
        }

     }
     else if( role == Qt::DecorationRole ) {
         if( index.column() == 1 ) {
                 // result/status

                    // broadcast always succeeds...
            if( _cmdHist.at(index.row()).broadcast ) {
                return QVariant(QIcon(":/icons/img/greenled.png"));

             }
             else {
                 if( _cmdHist.at(index.row()).status == OMC_QUEUED )
                     return QVariant(QIcon(":/icons/img/blueled.png"));
                 else if( _cmdHist.at(index.row()).status == OMC_FAILURE )
                     return QVariant(QIcon(":/icons/img/redled.png"));
                 else if( _cmdHist.at(index.row()).status == OMC_SUCCESS )
                     return QVariant(QIcon(":/icons/img/greenled.png"));
                 else
                     return QVariant(QIcon(":/icons/img/redled.png"));
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
     unsigned short dAddr;

        // use default strings and values for broadcast commands
     if(p_com.broadcast) {
         bName = "broadcast";
         dName = "broadcast";
         dAddr = 0;
     }
     else {
         bName = m_net->busInfo(p_com.network)->name;
         dName = m_net->deviceInfo(p_com.network, p_com.address)->name;
         dAddr = p_com.address;
     }

     QString cmd = p_com.command + " " + p_com.arguments.join(" ");


     slimHistoryEntry ent(p_com, bName, dName, dAddr, OMC_QUEUED, cmd, p_com.broadcast);

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

 void CommandHistoryModel::_commandCompleted(int p_id, OMCommandBuffer * p_buf) {


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

        // let the view(s) know that we changed some data here (perhaps)
     emit dataChanged(createIndex(thsRow, 0), createIndex(thsRow, 4));

        // if it's a command with a result value, then emit it for display
     if( p_buf->resultSize() > 0 ) {
         qDebug() << "SCHM: Command has result data bytes: " << p_buf->resultSize();
         emit commandResults(_cmdHist.at(thsRow));
     }

 }

 slimHistoryEntry CommandHistoryModel::getCommand(int p_row) {
     return _cmdHist.at(p_row);
 }
