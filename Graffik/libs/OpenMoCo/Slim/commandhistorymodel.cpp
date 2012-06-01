#include "commandhistorymodel.h"
#include <QDebug>
#include <QBrush>
#include <QMetaType>


CommandHistoryModel::CommandHistoryModel(OMNetwork *c_net, QObject *parent) :
    QAbstractTableModel(parent)
{
    m_net = c_net;

    qRegisterMetaType<slimCommand>("slimCommand");

        // we want the results of each command
    QObject::connect(m_net, SIGNAL(complete(OMCommandBuffer*)), this, SLOT(_commandCompleted(OMCommandBuffer*)), Qt::QueuedConnection);
}

int CommandHistoryModel::rowCount(const QModelIndex & parent) const {
        return _cmdVec.size();
 }

 int CommandHistoryModel::columnCount(const QModelIndex & parent) const {
     return 4; // 4 columns in slimCommand for display
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
            // get network name
            QString name = _cmdVec.at(index.row()).network;
            name = m_net->busInfo(name)->name;
            return QVariant(name);
        }
        else if( index.column() == 1 ) {
                // result/status

                // check, and then access command buffer for information
            OMCommandBuffer* buf = _cmdVec.at(index.row()).buf;

                // we MUST check that the buffer has been initialized (it likely is
                // not if the command was just added)
            if( buf != 0 ) {
                return QVariant(buf->status());
            }
            else {
                return QVariant("Q");
            }
        }
        else if( index.column() == 2 ) {
            // get device name
            QString name = m_net->deviceInfo(_cmdVec.at(index.row()).network, _cmdVec.at(index.row()).address)->name;
            return QVariant(name);
        }
        else if( index.column() == 3 ) {
            // command issued
            QString allCmd = _cmdVec.at(index.row()).command + " " + _cmdVec.at(index.row()).arguments.join(" ");
            return QVariant(allCmd);
        }

     }
     else if( role == Qt::BackgroundRole ) {
         if( index.column() == 0 ) {
             // color background of network area by user-specified color
             QColor bgCol = m_net->busInfo(_cmdVec.at(index.row()).network)->color;
             QBrush netBackground(bgCol);
             return(netBackground);
         }
         else if( index.column() == 1 ) {
             // change background color of status column

             OMCommandBuffer* buf = _cmdVec.at(index.row()).buf;


             if( buf == 0 || buf->status() == OMC_QUEUED ) {
                 // buffer may not be initialized yet
                 // or the command may only be queued at this point
                 QBrush grayBackground(Qt::gray);
                 return(grayBackground);
             }

             else if( buf->status() == OMC_FAILURE ) {
                QBrush redBackground(Qt::red);
                return(redBackground);
             }
             else {
                 QBrush greenBackground(Qt::green);
                 return(greenBackground);
             }
         }

     }

     return QVariant();
 }

 bool CommandHistoryModel::insertRow(slimCommand p_com) {


     beginInsertRows(QModelIndex(), this->rowCount(), this->rowCount());

     _cmdVec.push_back(p_com);

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

 void CommandHistoryModel::_commandCompleted(OMCommandBuffer * p_buf) {


     int thsId = p_buf->id();

        // strange, we have no record of that command! (Must've come from someone else)
     if( ! m_cmdLoc.contains(thsId) ) {
         // qDebug() << "SCHM: Ignoring command " << thsId;
         return;
     }

     // considering that we would've gotten this command with no actual OMCommandBuffer pointer, let's resolve
     // that

     int thsRow = m_cmdLoc[thsId];

     _cmdVec.at(thsRow).buf = p_buf;

     qDebug() << "SCHM: Command Result Size: " << p_buf->resultSize();

        // let the view(s) know that we changed some data here (perhaps)
     emit dataChanged(createIndex(thsRow, 0), createIndex(thsRow, 4));

        // if it's a command with a result value, then emit it for display
     if( p_buf->resultSize() > 0 ) {
         qDebug() << "SCHM: Command has result data bytes: " << p_buf->resultSize();
         emit commandResults(_cmdVec.at(thsRow));
     }

 }

 slimCommand CommandHistoryModel::getCommand(int p_row) {
     return _cmdVec.at(p_row);
 }
