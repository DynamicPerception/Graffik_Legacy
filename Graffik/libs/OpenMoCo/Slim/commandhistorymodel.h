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
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool insertRow(slimCommand);

signals:

    void commandResults(slimCommand);

public slots:

private slots:
    void _commandCompleted(OMCommandBuffer*);

private:
    QVector<slimCommand> _cmdVec;
    QHash<int, int> m_cmdLoc;
    OMNetwork* m_net;
};

#endif // COMMANDHISTORYMODEL_H
