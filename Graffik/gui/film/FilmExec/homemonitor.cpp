#include "homemonitor.h"

#include <QtEndian>
#include <QDebug>


HomeMonitor::HomeMonitor(OMNetwork *c_net, QObject *parent) : QObject(parent) {
    m_net = c_net;

    m_started = false;

        // list for commands completed
    connect(m_net, SIGNAL(complete(int,OMCommandBuffer*)), this, SLOT(_cmdReceived(int,OMCommandBuffer*)), Qt::QueuedConnection);

}

HomeMonitor::~HomeMonitor() {
    if( m_started )
        delete m_timer;
}

void HomeMonitor::start() {

    if( ! m_started ) {
        m_timer = new QTimer();
        connect(m_timer, SIGNAL(timeout()), this, SLOT(_timerFire()));
        m_timer->start(HMM_PERIOD);
        m_started = true;
    }

}

void HomeMonitor::stop() {
    if( m_started ) {
        m_timer->stop();
        delete m_timer;
        m_started = false;
    }
}


void HomeMonitor::checkAxes(QList<OMAxis *> p_axes) {
    m_axes = p_axes;
}

void HomeMonitor::_timerFire() {

        // on timer fire, we issue commands
        // to request distance from home from all
        // known axes.  We check and remove axes
        // with the command complete slot
    foreach(OMAxis* axis, m_axes) {
        qDebug() << "HM: Sending request for axis" << axis->address();
        int cmdId = axis->getHomeDist();
            // hold on to results!
        m_net->getManager()->hold(cmdId);
            // track commands issued
        m_cmds.insert(cmdId, axis);
    }

}

void HomeMonitor::_cmdReceived(int p_id, OMCommandBuffer *p_cmd) {
    // received command completed from network manager

    if( ! m_cmds.contains(p_id) )
        return;

    if( p_cmd->status() == OMC_SUCCESS ) {
        long distance = 0;
        unsigned int resSize = p_cmd->resultSize();

        if( resSize > 0 ) {

            char* res = new char[resSize];
            p_cmd->result(res, resSize);

            distance = abs(qFromBigEndian<qint32>((uchar*)res));
            delete res;

            qDebug() << "HM: Found current position for axis" << m_cmds.value(p_id)->address() << "is" << distance;

            if( distance < HMM_SLOP ) {
                    // node is at home, remove it from list
                    // of axes we're monitoring
                m_axes.removeAll(m_cmds.value(p_id));
                qDebug() << "HM: Device is at home";
            }

        } // end if( resSize > 0...
    }
    else {
        QString errText = "Received Error Sending Device " + m_net->getDevices().value(m_axes.value(p_id)->address())->name + " Home";
        emit error(errText);
    }

    m_cmds.remove(p_id);
    m_net->getManager()->release(p_id);

        // no more axes left, we can emit a signal
        // informing that all nodes are at home!
    if( m_axes.count() < 1 )
        emit allAtHome();


}
