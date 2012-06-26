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
