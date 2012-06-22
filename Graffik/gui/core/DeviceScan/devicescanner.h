#ifndef DEVICESCANNER_H
#define DEVICESCANNER_H

#include <QObject>

#include "deviceassigndialog.h"
#include "devicescandialog.h"
#include "MoCoBus/omnetwork.h"

class DeviceScanner : public QObject
{
    Q_OBJECT
public:

    DeviceScanner(OMNetwork* c_net, QObject *parent = 0);
    DeviceScanner(OMNetwork *c_net, unsigned short c_addr, QObject *parent = 0);
    ~DeviceScanner();

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
        devInfo(QString c_bus, QString c_id, unsigned short c_addr) {
            bus = c_bus;
            id = c_id;
            addr = c_addr;
        }
    };

    OMNetwork* m_net;
    OMCommandManager* m_cmd;
    DeviceScanDialog* m_scanDialog;
    QHash<int, unsigned short> m_cmdSent;
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

};

#endif // DEVICESCANNER_H
