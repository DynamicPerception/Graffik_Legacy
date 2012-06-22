#include "slimcommandparser.h"

#include <QRegExp>
#include <QDebug>


SlimCommandParser::SlimCommandParser(OMNetwork *c_net)
{
    m_net = c_net;
    m_dNet = "COM1";
    m_dAddr = 2;
}


/** Parse Command From String

  Parses a scripting command and sends it to the indicated device
  if such a command is supported by the indicated device.

  If the command begins with an alias for a device, it is issued
  to that device - otherwise, the default device port and address
  is used.

  @throw SLIM_ERR_EMPTY
  No command was given

  @throw SLIM_ERR_NODEV
  No such device is registered

  @throw SLIM_ERR_NOCMD
  No such command associated with device

  @return
  A slimCommand structure representing the command issued.

  */

slimCommand SlimCommandParser::parse(QString & p_String)
{
    slimCommand newCom;
    QString devKey;
    QString cmd;
    SlimScriptDevice* dev;
    bool bcast = false;
    int id;

    unsigned short addr;
    QString net;

    if( p_String == 0 || p_String.length() <= 0 )
        throw SLIM_ERR_EMPTY;


    p_String = p_String.trimmed();

    QStringList cmdWords = p_String.split(QRegExp("\\s+"));


    if( cmdWords.size() < 1 )
        throw SLIM_ERR_EMPTY;


        // handle broadcast commands specially
    if( cmdWords[0] == "broadcast" ) {
        net = "Broadcast";
        addr = OMBus::OM_SER_BCAST_ADDR;
        cmdWords.removeFirst();
        bcast = true;
    }
    else {

            // is first word in command an alias for a device to speak to?
        if( m_aliases.contains(cmdWords[0]) ) {
            qDebug() << "SLCP: Got Alias: " << cmdWords[0];
                // get address key from aliases
                // list and then remove the alias from the command
            devKey = m_aliases.value(cmdWords[0]);

            QStringList target = devKey.split(QRegExp(":+"));
            net = target[0];
            addr = target[1].toUShort();

            cmdWords.removeFirst();
        }

        else {
            net = m_dNet;
            addr = m_dAddr;
            devKey = net + ":" + QString::number(addr);

        }
    } // end if not broadcast

        // get first word as command
    cmd = cmdWords[0];
    cmdWords.removeFirst();

    if( ! bcast ) {

        if( m_regDevs.contains(devKey) != true )
            throw SLIM_ERR_NODEV;

        dev = m_regDevs.value(devKey);

        newCom.broadcast = false;

            // the command isn't supported by this device?
        if( ! dev->namedCommandExists(cmd) )
            throw SLIM_ERR_NOCMD;


            // catch and forward any exceptions from the
            // runCommand method in the SlimDevice object
        try {
            id = dev->runNamedCommand(cmd, &cmdWords);
        }
        catch (int exc) {
            throw exc;
        }
    }
    else {
        newCom.broadcast = true;
            // attempt to send a broadcast command
        try {
            id = _sendBroadcast(cmd);
        }
        catch( int exc ) {
            qDebug() << "SLCP: Received exception from bcast: " << exc;
            throw exc;
        }
    }

    // populate slimCommand structure
    newCom.buf = 0; // notice this! better check it before you use it!
    newCom.device = dev;
    newCom.id = id;
    newCom.network = net;
    newCom.address = addr;
    newCom.command = cmd;
    newCom.arguments = cmdWords;

        // return slimCommand
    return newCom;
}

const int SlimCommandParser::_sendBroadcast(QString p_cmd) {

    qDebug() << "SLCP: Got broadcast request" << p_cmd;

    unsigned int cmd;

    if( p_cmd == "start" )
        cmd = OMBus::OM_BCAST_START;
    else if( p_cmd == "stop" )
        cmd = OMBus::OM_BCAST_STOP;
    else if( p_cmd == "pause" )
        cmd = OMBus::OM_BCAST_PAUSE;
    else
        throw SLIM_ERR_NOCMD;

    return m_net->broadcast(cmd);

}

/** Register A New Device with the Parser

  */

void SlimCommandParser::registerDevice(SlimScriptDevice* p_dev, QString p_port, unsigned short p_addr)
{

    QString addr = QString::number(p_addr);
    QString devKey = p_port + ":" + addr;

    qDebug() << "SLCP: Register Device " << devKey;

    m_regDevs.insert(devKey,p_dev);
}

/** Set Default Network and Address

  */

void SlimCommandParser::defaultNetAddr(QString p_port, unsigned short p_addr)
{
    m_dNet = p_port;
    m_dAddr = p_addr;
}

/** Add An Alias

  */

void SlimCommandParser::addAlias(QString p_alias, QString p_port, unsigned short p_addr) {
    QString devKey = p_port + ":" + QString::number(p_addr);
    m_aliases.insert(p_alias, devKey);
}

/** Remove a Device

  */

void SlimCommandParser::removeDevice(QString p_port, unsigned short p_addr) {
    QString devKey = p_port + ":" + QString::number(p_addr);
    QString alias = m_aliases.key(devKey);
    m_aliases.remove(alias);
    m_regDevs.remove(devKey);
}
