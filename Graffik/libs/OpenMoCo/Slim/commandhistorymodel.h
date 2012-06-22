#ifndef COMMANDHISTORYMODEL_H
#define COMMANDHISTORYMODEL_H

#include <QAbstractTableModel>
#include <QString>
#include <QVector>

#include "slimcommandparser.h"
#include "MoCoBus/omnetwork.h"

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

    slimCommand getCommand(int p_row);

    bool insertRow(slimCommand);

signals:

    void commandResults(slimCommand);

public slots:

private slots:
    void _commandCompleted(int p_id, OMCommandBuffer* p_buf);
    void _deviceRemoved(QString p_bus, unsigned short p_addr);

private:
    QVector<slimCommand> _cmdVec;
    QHash<int, int> m_cmdLoc;
    QHash<unsigned short, QList<int> > m_devCmdLookup;

    OMNetwork* m_net;
    QColor* m_bcCol;
    QString m_headers;
};

#endif // COMMANDHISTORYMODEL_H
