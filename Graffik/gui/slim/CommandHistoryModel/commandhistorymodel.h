#ifndef COMMANDHISTORYMODEL_H
#define COMMANDHISTORYMODEL_H

#include <QAbstractTableModel>
#include <QString>
#include <QList>

#include "Slim/slimcommandparser.h"
#include "MoCoBus/omnetwork.h"

    /** An entry in the history of commands issued via slim
      */
struct slimHistoryEntry {
        /** slimCommand object */
    mutable slimCommand cmdObject;
        /** bus name */
    QString bus;
        /** device name */
    QString deviceName;
        /** device address */
    unsigned short deviceAddr;
        /** command status */
    mutable int status;
        /** command and argument text */
    QString command;
        /** Broadcast command ? */
    bool broadcast;

        /** constructor */
    slimHistoryEntry(slimCommand c_cmd, QString c_bus, QString c_devName, unsigned short c_devAddr, int c_stat, QString c_com, bool p_bcast) {
        cmdObject = c_cmd;
        bus = c_bus;
        deviceName = c_devName;
        deviceAddr = c_devAddr;
        status = c_stat;
        command = c_com;
        broadcast = p_bcast;
    }
};

class CommandHistoryModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    CommandHistoryModel(OMNetwork*, QObject *parent);
    ~CommandHistoryModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    slimHistoryEntry getCommand(int p_row);

    bool insertRow(slimCommand);

signals:

    void commandResults(slimHistoryEntry);

public slots:

private slots:
    void _commandCompleted(int p_id, OMCommandBuffer* p_buf);

private:
    QList<slimHistoryEntry> _cmdHist;
    QHash<int, int> m_cmdLoc;

    OMNetwork* m_net;
    QString m_headers;
};

#endif // COMMANDHISTORYMODEL_H
