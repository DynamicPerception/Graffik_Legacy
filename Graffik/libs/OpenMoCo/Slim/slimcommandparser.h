#ifndef SLIMCOMMANDPARSER_H
#define SLIMCOMMANDPARSER_H

#include <QString>
#include <QStringList>
#include <QList>
#include <QHash>


#include "openmoco.h"
#include "slimscriptdevice.h"
#include "MoCoBus/omcommandbuffer.h"


#define SLIM_ERR_EMPTY  200
#define SLIM_ERR_NOCMD   201
#define SLIM_ERR_ARG   202
#define SLIM_ERR_ARGS   203
#define SLIM_ERR_NODEV  204

struct slimCommand {
    int id;
    unsigned short address;
    QString network;
    QString command;
    QStringList arguments;
    SlimScriptDevice* device;
    mutable OMCommandBuffer* buf;
};


class SlimCommandParser : public OpenMoCo
{
public:
    SlimCommandParser();

    slimCommand parse(QString &);
    void registerDevice(SlimScriptDevice*, QString, unsigned short);

    void defaultNetAddr(QString, unsigned short);
    void addAlias(QString, QString, unsigned short);


private:

    QHash<QString, SlimScriptDevice*> m_regDevs;
    QHash<QString, QString> m_aliases;

    QString m_dNet;
    unsigned short m_dAddr;
};

#endif // SLIMCOMMANDPARSER_H
