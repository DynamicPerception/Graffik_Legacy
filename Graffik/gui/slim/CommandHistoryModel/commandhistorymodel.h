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

#ifndef COMMANDHISTORYMODEL_H
#define COMMANDHISTORYMODEL_H

#include <QAbstractTableModel>
#include <QString>
#include <QList>
#include <QtEndian>
#include <QBrush>
#include <QMetaType>
#include <QIcon>
#include <QPixmap>

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
        /** Result Text String */
    mutable QString* commandResult;

        /** constructor */
    slimHistoryEntry(slimCommand c_cmd, QString c_bus, QString c_devName, unsigned short c_devAddr, int c_stat, QString c_com, bool p_bcast, QString* c_res) {
        cmdObject = c_cmd;
        bus = c_bus;
        deviceName = c_devName;
        deviceAddr = c_devAddr;
        status = c_stat;
        command = c_com;
        broadcast = p_bcast;
        commandResult = c_res;
    }
};

/** Command History Model

  The CHM provides a complete history of every command entered via Slim by the user.

  @author
  C. A. Church
  */

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

    QPixmap errLED();
    QPixmap okLED();
    QPixmap queueLED();
    QPixmap bcastLED();

    void setErrLED(QPixmap p_pix);
    void setOkLED(QPixmap p_pix);
    void setQueueLED(QPixmap p_pix);
    void setBcastLED(QPixmap p_pix);


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

    QString* m_resQueued;
    QString* m_resBcast;

    QPixmap* m_errLed;
    QPixmap* m_okLed;
    QPixmap* m_queLed;
    QPixmap* m_bcLed;

    QList<QString*> m_createdStrings;

    void _processResults(int p_row, OMCommandBuffer *p_buf);
};

#endif // COMMANDHISTORYMODEL_H
