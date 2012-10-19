/*
    Dynamic Perception MoCoBus Development Library
    Copyright (c) 2010-2012 Dynamic Perception LLC

    This file is part of The Dynamic Perception MoCoBus Development Library.

    The Dynamic Perception MoCoBus Development Library is free software:
    you can redistribute it and/or modify it under the terms of the GNU
    Lesser General Public License as published by the Free Software Foundation,
    either version 3 of the License, or (at your option) any later version.

    The Dynamic Perception MoCoBus Development Library is distributed in the
    hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
    See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with The Dynamic Perception MoCoBus Development Library.  If not,
    see <http://www.gnu.org/licenses/>.

  */

#ifndef SLIMCOMMANDPARSER_H
#define SLIMCOMMANDPARSER_H

#include <QString>
#include <QStringList>
#include <QList>
#include <QHash>


#include "openmoco.h"
#include "slimscriptdevice.h"
#include "MoCoBus/omcommandbuffer.h"
#include "MoCoBus/omnetwork.h"


/** Slim Command Parser Class

   Provides the ability to parse textual commands for MoCoBus devices

   @author
   C. A. Church

  (c) 2011-2012 Dynamic Perception LLC
  Licensed under the terms of the GNU LGPL version 3.
  */


struct slimCommand {
    int id;
    unsigned short address;
    bool broadcast;
    QString network;
    QString command;
    QStringList arguments;
    SlimScriptDevice* device;
    mutable OMCommandBuffer* buf;
};


class SlimCommandParser
{
public:
    SlimCommandParser(OMNetwork* c_net);

    slimCommand parse(QString &);
    void registerDevice(SlimScriptDevice*, QString, unsigned short);
    void removeDevice(QString, unsigned short);

    void defaultNetAddr(QString, unsigned short);
    void addAlias(QString, QString, unsigned short);


private:

    const int _sendBroadcast(QString);

    QHash<QString, SlimScriptDevice*> m_regDevs;
    QHash<QString, QString> m_aliases;

    QString m_dNet;
    unsigned short m_dAddr;
    OMNetwork* m_net;

};

#endif // SLIMCOMMANDPARSER_H
