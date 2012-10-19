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

#ifndef SLIMSCRIPTDEVICE_H
#define SLIMSCRIPTDEVICE_H


#include <QMap>
#include <QString>

#define SLIM_ERR_EMPTY  200
#define SLIM_ERR_NOCMD   201
#define SLIM_ERR_ARG   202
#define SLIM_ERR_ARGS   203
#define SLIM_ERR_NODEV  204
#define SLIM_ERR_NOBUS  205

/** Associate a Slim Scripting Language with an OMDevice

   The SlimScriptDevice class allows one to associate a set of
   named commands with a device, for use in the Scripting GUI or
   other applications that require a simple, string-based set of
   scripting commands for input by a human actor.

   Inheriting from this class provides the necessary methods and
   data types for your device to add and manage these commands, and
   all necessary functionally to register the device with the SlimCommandParser.

   For more information on using this class, see the addNamedCommand() documentation.

   @author
   C. A. Church

  (c) 2011-2012 Dynamic Perception LLC
  Licensed under the terms of the GNU LGPL version 3.
   */

class SlimScriptDevice
{

public:
    SlimScriptDevice();
    
    bool namedCommandExists(QString);
    const int runNamedCommand(QString, QStringList*);

protected:
    typedef const int(SlimScriptDevice::*f_callBack)(QStringList&);

    void addNamedCommand(QString, f_callBack);
    void delNamedCommand(QString);

private:
    // command list is command name -> handler
   QMap<QString, f_callBack> m_cmdList;

};

#endif // SLIMSCRIPTDEVICE_H
