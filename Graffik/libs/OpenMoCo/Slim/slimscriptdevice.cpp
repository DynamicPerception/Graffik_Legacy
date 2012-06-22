#include "slimscriptdevice.h"

SlimScriptDevice::SlimScriptDevice()
{
}

/*
     Named Commands
*/


/** Add Named Command

  Adds a named command to this device.  This is used for scripting, and
  other functions which require a generic named string identifying a
  command to send the device, and whose remaining arguments are a list of
  QStrings.

  This method takes two parameters, a QString with the command name and
  a method pointer which matches the SlimScriptDevice::f_callback type.  Because
  of the type of callback allowed, only methods derived from classes which
  are subclasses of SlimScriptDevice may be passed, and they must be statically cast
  into the SlimScriptDevice namespace.

  This method pointer will be called, with a QStringList argument, when
  runCommand() is called with the specified name.

  The following is an example of defining a scripting language for a device,
  with three commands mapped back to three methods for handling them:

  MyDevice.h:
  @code

  #include <QStringList>
  #include "MoCoBus/omdevice.h"
  #include "Slim/slimscriptdevice.h"

  class MyDevice : public OMDevice, public SlimScriptDevice {

  public:

    MyDevice(OMMoCoBus*, unsigned short);
    void led(bool);
    void start();
    void stop();

  private:

    void _scriptLed(QStringList);
    void _scriptStart(QStringList);
    void _scriptStop(QStringList);
  };
  @endcode

  MyDevice.cpp:
  @code

  #include <QDebug>
  #include "MyDevice.h"

  MyDevice::MyDevice(OMMoCoBus* c_bus, unsigned short c_addr) : OMDevice(c_bus, c_addr), SlimScriptDevice() {

        / define the device, as per OMDevice
    deviceVersion = 1;
    deviceType = "MyDevice";

        // add three scripting commands
    addCommand("start", static_cast<f_callBack>(&MyDevice::_scriptStart));
    addCommand("stop", static_cast<f_callBack>(&MyDevice::_scriptStop));
    addCommand("led", static_cast<f_callBack>(&MyDevice::_scriptLed));
  }

  void MyDevice::led(bool p_on) {
    // do something
  }

  void MyDevice::start() {
    // do something
  }

  void MyDevice::stop() {
    // do something
  }

  void MyDevice::_scriptLed(QStringList p_args) {

        // not enough arguments
    if( p_args.isEmpty() )
        throw OM_ERR_ARGS;

    if( p_args[0] == "on" ) {
        qDebug() << "Got LED On";
        this->led(true);
    }
    else if( p_args[0] == "off" ) {
        qDebug() << "Got LED Off";
        this->led(false);
    }
    else {
            // invalid argument received
        throw OM_ERR_ARG;
    }
 }

 void MyDevice::_scriptStart(QStringList p_args) {
    this->start();
 }

 void MyDevice::_scriptStop(QStringList p_args) {
    this->stop();
 }
 @endcode

 @param p_cmd
 A QString containing the name of the script command

 @param p_callBack
 The callback method pointer to associate with the command.
 */

void SlimScriptDevice::addNamedCommand(QString p_cmd, f_callBack p_callBack) {
    m_cmdList.insert(p_cmd,p_callBack);
}

/** Named Command Exists

  Returns true if the named command has been added to this device, false
  otherwise.

  @param p_cmd
  A QString containing the name of the command

  @return
  Boolean value, true if the command exists, false if not
  */

bool SlimScriptDevice::namedCommandExists(QString p_cmd) {
    return m_cmdList.contains(p_cmd);
}

/** Delete Named Command

  Deletes the specified named command from the list of named commands.

  If the specified command does not exist, does nothing.

  @param p_cmd
  A QString containing the name of the command
  */

void SlimScriptDevice::delNamedCommand(QString p_cmd) {

    if( m_cmdList.contains(p_cmd) )
        m_cmdList.remove(p_cmd);

}

/** Run Named Command

  Runs the named command by executing the method pointer associated with
  the named command, passing along the specified list of arguments.

  If the specified named command does not exist, this method does nothing.

  If the method executed via the pointer throws any signal, it is captured
  and passed back to the called of this method.

  @param p_cmd
  QString containing the name of the command

  @param p_args
  A pointer to a QStringList of arguments.

  @return
  The unique ID of the command issued to the bus
  */

const int SlimScriptDevice::runNamedCommand(QString p_cmd, QStringList* p_args) {
    int id;
    if( this->namedCommandExists(p_cmd) == true ) {
        f_callBack thsFunc = m_cmdList[p_cmd];
        try {
            id = (this->*thsFunc)(*p_args);
        }
        catch (int e) {
            throw e;
        }
    } // end if

    return id;
}

