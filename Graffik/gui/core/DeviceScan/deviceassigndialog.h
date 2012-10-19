#ifndef DEVICEASSIGNDIALOG_H
#define DEVICEASSIGNDIALOG_H

#include <QDialog>
#include <QString>

#include "MoCoBus/omnetwork.h"
#include "Devices/nanoMoCo/omaxis.h"

namespace Ui {
class DeviceAssignDialog;
}

class DeviceAssignDialog : public QDialog
{
    Q_OBJECT
    
public:
    DeviceAssignDialog(OMNetwork* c_net, QString c_bus, QString c_type, QString c_name, QWidget *parent = 0);
    ~DeviceAssignDialog();
    
public slots:
    void accept();

private slots:
    void _commandComplete(int p_id, OMCommandBuffer* p_cmd);

private:
    Ui::DeviceAssignDialog *ui;

    OMNetwork* m_net;
    QString m_bus;
    QString m_type;
    QString m_name;
    int m_cmdId;
    QString m_newName;
    unsigned short m_newAddr;

    QHash<int, unsigned short> m_cmdSent;

    void _changeAddr(unsigned short p_old, unsigned short p_new);
    void _initInputs(QString p_type);
    void _postInit(QString p_bus, unsigned short p_addr, QString p_type, QString p_name);
};

#endif // DEVICEASSIGNDIALOG_H
