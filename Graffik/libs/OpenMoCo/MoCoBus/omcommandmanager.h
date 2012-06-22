#ifndef OMCOMMANDMANAGER_H
#define OMCOMMANDMANAGER_H

#include <QObject>
#include <QHash>
#include <QMutex>

#include "openmoco.h"
#include "omcommandbuffer.h"

/**

  The OMCommandManager provides a means for managing the lifecycle (and
  garbage collection) of OMCommandBuffer objects when there are multiple
  objects submitting commands to one or more devices.

  As the results of commands issued to devices are transmitted using a
  signal and slot methodology, it is not possible for the OMNetwork, OMMoCoBus,
  or the OMSerialMgr objects to manage the lifecycle of the resulting
  OMCommandBuffer objects themselves. It is also not possible for any one
  consuming object to manage the lifecycle of these objects directly without a complete
  picture of their use in other objects.  The OMCommandManager fulfills this purpose
  by providing a central repository of all created OMCommandBuffer objects, and a
  means for consuming objects to register their requirements to hold on to objects,
  or release them when done.

  <b>WARNING: When this object is destroyed, all commands issued by the associated OMNetwork
  are freed from memory.</b>

  @author C. A. Church

  (c) 2012 Dynamic Perception LLC
  Licensed under the terms of the GNU LGPL version 3.

  */

class OMCommandManager : public QObject
{
    Q_OBJECT
public:
    explicit OMCommandManager(QObject *parent);
    ~OMCommandManager();

    void hold(const int p_cmdID);
    void release(const int p_cmdID);
    
signals:
    
public slots:
    void registerCommand(OMCommandBuffer* p_cmd);

private slots:

private:
    QHash<int, OMCommandBuffer*> m_comList;
    QHash<int, unsigned int> m_comHoldCnt;
    
    QMutex* m_cntMutex;

};

#endif // OMCOMMANDMANAGER_H
