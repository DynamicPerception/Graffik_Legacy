#include "commandhistorymodel.h"
#include <QDebug>
#include <QBrush>
#include <QMetaType>
#include <QIcon>


CommandHistoryModel::CommandHistoryModel(OMNetwork *c_net, QObject *parent) :
    QAbstractTableModel(parent)
{
    m_net = c_net;
    m_bcCol = new QColor("white");


    qRegisterMetaType<slimCommand>("slimCommand");

        // we want the results of each command
    QObject::connect(m_net, SIGNAL(complete(int, OMCommandBuffer*)), this, SLOT(_commandCompleted(int, OMCommandBuffer*)), Qt::QueuedConnection);

}

CommandHistoryModel::~CommandHistoryModel() {
    delete m_bcCol;
}

int CommandHistoryModel::rowCount(const QModelIndex & parent) const {
        return _cmdHist.size();
 }

 int CommandHistoryModel::columnCount(const QModelIndex & parent) const {
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
            // get network name
            if( _cmdHist.at(index.row()).broadcast ) {
                    // handle broadcast packets
                QString name = "broadcast";
                return QVariant(name);
            }
            else {
                return QVariant(_cmdHist.at(index.row()).bus);
            }
        }

        else if( index.column() == 2 ) {
            // get device name
            if( _cmdHist.at(index.row()).broadcast )
                return QVariant("broadcast");

            return QVariant(_cmdHist.at(index.row()).deviceName);
        }
        else if( index.column() == 3 ) {
            // command issued
            QString allCmd = _cmdHist.at(index.row()).command;
            return QVariant(allCmd);
        }

     }
     else if( role == Qt::DecorationRole ) {
         if( index.column() == 1 ) {
                 // result/status

            if( _cmdHist.at(index.row()).broadcast ) {
                return QVariant(QIcon(":/icons/img/greenled.png"));

             }
             else {
                    // we MUST check that the buffer has been initialized (it likely is
                    // not if the command was just added)
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
     else if( role == Qt::BackgroundRole ) {

         if( index.column() == 0 ) {
             // color background of network area by user-specified color

             QColor bgCol = *m_bcCol;

             if( ! _cmdHist.at(index.row()).broadcast )
                 bgCol = _cmdHist.at(index.row()).color;

             QBrush netBackground(bgCol);
             return(netBackground);
         }

     }

     return QVariant();
 }

 bool CommandHistoryModel::insertRow(slimCommand p_com) {


     beginInsertRows(QModelIndex(), this->rowCount(), this->rowCount());

     QString bName = m_net->busInfo(p_com.network)->name;
     QColor bCol = m_net->busInfo(p_com.network)->color;
     QString dName = m_net->deviceInfo(p_com.network, p_com.address)->name;
     QString cmd = p_com.command + " " + p_com.arguments.join(" ");

     slimHistoryEntry ent(p_com, bName, bCol, dName, p_com.address, OMC_QUEUED, cmd, p_com.broadcast);

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

     qDebug() << "SCHM: Received complete signal";

        // strange, we have no record of that command! (Must've come from someone else)
     if( ! m_cmdLoc.contains(p_id) ) {
         qDebug() << "SCHM: Ignoring command " << p_id;
         return;
     }

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

 void CommandHistoryModel::_deviceRemoved(QString p_bus, unsigned short p_addr) {

    // deprecated

 }
