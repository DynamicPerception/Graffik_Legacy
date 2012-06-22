#include "omcommandmanager.h"
#include <QList>
#include <QDebug>


OMCommandManager::OMCommandManager(QObject *parent) :
    QObject(parent)
{

    m_cntMutex = new QMutex;

}


OMCommandManager::~OMCommandManager() {
    int key;

    QList<int> keys = m_comList.keys();

    foreach( key, keys ) {
        OMCommandBuffer* ths = m_comList.value(key);
        delete ths;
        m_comList.remove(key);
    }

    delete m_cntMutex;
}

/** Hold onto a Command Object

  Indicates that you are using the OMCommandBuffer object, and it should not ne
  freed from memory (yet).  Call release() when you are done with the object

  @param p_cmdID
  The command ID to hold on to
  */

void OMCommandManager::hold(const int p_cmdID) {

   m_cntMutex->lock();

    if( m_comHoldCnt.contains(p_cmdID) ) {
        m_comHoldCnt[p_cmdID]++;
    }
    else {
        m_comHoldCnt.insert(p_cmdID, 1);
    }

   m_cntMutex->unlock();
}

/** Release a Hold on a Command Object

  Indicates that you are no longer using the OMCommandBuffer object, and it could be
  freed from memory.  You must not use any pointer to the OMCommandBuffer object
  referenced after calling this method.

  Once all holds have been released, the OMCommandBuffer object will be freed from
  memory.

  @param p_cmdID
  The command ID to free
  */

void OMCommandManager::release(const int p_cmdID) {

   m_cntMutex->lock();

    if( m_comHoldCnt.contains(p_cmdID) ) {
        m_comHoldCnt[p_cmdID]--;

        if( m_comHoldCnt[p_cmdID] <= 0 ) {
            if( m_comList.contains(p_cmdID)) {
                OMCommandBuffer* ths = m_comList.value(p_cmdID);
                delete ths;
            }
            m_comHoldCnt.remove(p_cmdID);
            m_comList.remove(p_cmdID);

        } // end if( .. count <= 0
    } // end if contains command id

   m_cntMutex->unlock();
}

/** Register Command Slot

  This Slot is to be called upon completion of a command.  It stores
  the command buffer object for management if any actor has requested
  a hold on it by the time the command has completed.  If no actor has
  requested a hold on the command, it is immediately freed.

  @param p_cmd
  A pointer to the OMCommandBuffer object
  */


void OMCommandManager::registerCommand(OMCommandBuffer* p_cmd) {


    m_cntMutex->lock();

    const int cmdId = p_cmd->id();

    if( m_comHoldCnt.contains(cmdId) && m_comHoldCnt.value(cmdId) > 0 ) {
        m_comList.insert(cmdId, p_cmd);
        m_cntMutex->unlock();
     //   qDebug() << "OMCM: Retaining command id" << p_cmd->id();
    }
    else {
            // we delete the object if nobody has laid claim to it
            // by the time the command completes.
        m_cntMutex->unlock();
      //  qDebug() << "OMCM: NOT Retaining command id" << p_cmd->id();
        delete p_cmd;
    }



}
