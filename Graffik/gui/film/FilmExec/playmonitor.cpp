#include "playmonitor.h"


#include <QDebug>
#include <QtEndian>

PlayMonitor::PlayMonitor(OMNetwork *c_net, FilmParameters *c_film, QObject *parent) : QObject(parent) {
    m_film = c_film;
    m_net = c_net;
    m_started = false;

        // listen for commands completed
    connect(m_net, SIGNAL(complete(int,OMCommandBuffer*)), this, SLOT(_cmdReceived(int,OMCommandBuffer*)), Qt::QueuedConnection);

}

PlayMonitor::~PlayMonitor() {
    if( m_started )
        delete m_timer;
}

void PlayMonitor::start() {

    if( ! m_started ) {
        m_timer = new QTimer();
        connect(m_timer, SIGNAL(timeout()), this, SLOT(_timerFire()));
        m_timer->start(PM_TIMER_PERIOD);
        m_started = true;
    }

}

void PlayMonitor::stop() {
    if( m_started == true ) {
        m_timer->stop();
        delete m_timer;
        m_started = false;
    }
}


void PlayMonitor::master(OMAxis *p_master) {
    m_master = p_master;
}

void PlayMonitor::_timerFire() {

        // on timer fire, we issue a command to the
        // master device to request its run status, and
        // another to request its run time.

        // don't do anything if a master isn't set..
    if( m_master == 0 )
        return;


    int cmdId = m_master->getRunning();
    m_net->getManager()->hold(cmdId);
    m_cmds.insert(cmdId, false);

    cmdId = m_master->getRunTime();
    m_net->getManager()->hold(cmdId);
    m_cmds.insert(cmdId, true);

    m_gotCount = 0;
    m_runStat  = false;
    m_runTime  = 0;

}

void PlayMonitor::_cmdReceived(int p_id, OMCommandBuffer *p_cmd) {

    if( ! m_cmds.contains(p_id) )
        return;

    qDebug() << "PM: Recv'd back " << p_id;

    if( p_cmd->status() == OMC_SUCCESS ) {
        unsigned int resSize = p_cmd->resultSize();

        if( resSize > 0 ) {

            char* res = new char[resSize];
            p_cmd->result(res, resSize);

                // place value in right place..
            if( m_cmds.value(p_id) == false ) {
                m_runStat = res[0];
            }
            else {
                m_runTime = qFromBigEndian<quint32>((uchar*)res);
            }

            delete res;
            m_gotCount++;

        } // end if( resSize > 0...
    }
    else {

        qDebug() << "PM: Got Err" << p_cmd->status();

        QString errText = "Received Error Monitoring Master Device " + m_net->getDevices().value(m_master->address())->name + " " + QString::number(p_cmd->status()) + ", Film Aborted";
        emit error(errText);

    }

    m_cmds.remove(p_id);
    m_net->getManager()->release(p_id);

    if( m_gotCount > 1 )
        playStatus(m_runStat, m_runTime);


}
