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

#ifndef DEVICESCANNER_H
#define DEVICESCANNER_H

#include <QObject>

#include "deviceassigndialog.h"
#include "devicescanwidget.h"
#include "MoCoBus/omnetwork.h"
#include "Devices/nanoMoCo/omaxis.h"


#define OMDS_IDQ    0
#define OMDS_NMQ    1

/** Device Scanner Class

  The Device Scanner looks for devices on all buses - either for any device
  by scanning the complete address range, or for a specific device when constructed
  with an address argument.

  Instances should be destroyed when scanning is complete, do not attempt to use the same
  instance for scanning twice.

  @author
  C. A. Church
  */

class DeviceScanner : public QObject
{
    Q_OBJECT
public:

    DeviceScanner(OMNetwork* c_net, bool c_start = false, QObject *parent = 0);
    DeviceScanner(OMNetwork *c_net, unsigned short c_addr, bool c_start = false, QObject *parent = 0);
    ~DeviceScanner();

    DeviceScanWidget* getWidget();
    bool startScan();

signals:

public slots:
    
private slots:
    void _commandCompleted(int p_id, OMCommandBuffer* p_com);
    void _scanAccepted();

private:

    struct devInfo {
        QString bus;
        QString id;
        unsigned short addr;
        QString name;
        devInfo(QString c_bus, QString c_id, unsigned short c_addr, QString c_name) {
            bus = c_bus;
            id = c_id;
            addr = c_addr;
            name = c_name;
        }
    };

    OMNetwork* m_net;
    OMCommandManager* m_cmd;
    DeviceScanWidget* m_scanWidget;
        // list contains address sent to, and type (id or name)
    QHash<int, QList<unsigned short> > m_cmdSent;
    QHash<unsigned short, QString> m_addrType;

    bool m_scanStarted;

    QHash<int, QString> m_cmdSentBus;
    QList<devInfo> m_foundDevs;
    int m_respCount;
    int m_foundCount;
    unsigned short m_findAddr;

    bool _validateAddress(unsigned short p_addr);
    void _initScanner(OMNetwork* c_net, unsigned short p_addr);

    void _checkDone();

    void _scan(unsigned short p_addr);
    void _sendRequest(QString p_bus, unsigned short p_addr);


    bool _probe(QString p_bus, unsigned short p_addr, QString p_type);


};

#endif // DEVICESCANNER_H
