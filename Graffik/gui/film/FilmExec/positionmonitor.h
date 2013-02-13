/*

  Graffik Motion Control Application

  Copyright (c) 2011-2013 Dynamic Perception

 This file is part of Graffik.

    Graffik is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Graffik is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Graffik.  If not, see <http://www.gnu.org/licenses/>.

    */

#ifndef HOMEMONITOR_H
#define HOMEMONITOR_H

#include <QObject>
#include <QList>
#include <QHash>
#include <QTimer>
#include <QtEndian>

#include "MoCoBus/omnetwork.h"
#include "Devices/nanoMoCo/omaxis.h"

#include "core/Options/globaloptions.h"


const int HMM_PERIOD = 500;
const unsigned long HMM_SLOP   = 10;

/** Position Monitor Class

  The Position Monitor Class monitors a specified set of axes, reporting
  the allAtPosition() signal when all axes have arrived at the desired position.

  This class uses a QTimer to periodically check the position of each specified
  axis by querying it for its distance from home.

  If any error occurs, the error() signal will be emitted.

  @author C. A. Church
  */

class PositionMonitor : public QObject
{
    Q_OBJECT
public:
    explicit PositionMonitor(OMNetwork* c_net, GlobalOptions* c_gopts, QObject *parent = 0);
    ~PositionMonitor();


        /** Monitor these Axes */
    void checkAxes(QHash<OMAxis*, unsigned long> p_axes);

        /** Start the QTimer */
    void start();
        /** Stop the QTimer */
    void stop();

signals:
        /** All Monitored Axes are at Home */
    void allAtPosition();
        /** An Error Occurred */
    void error(QString p_err);

public slots:

private slots:
    void _timerFire();
    void _cmdReceived(int p_id, OMCommandBuffer* p_cmd);

private:

    OMNetwork* m_net;
    QTimer* m_timer;
    GlobalOptions* m_gopts;

    QHash<OMAxis*, unsigned long> m_axes;
    QHash<int, OMAxis*> m_cmds;
    QHash<unsigned short, unsigned long> m_positions;

    bool m_started;

};

#endif // HOMEMONITOR_H
