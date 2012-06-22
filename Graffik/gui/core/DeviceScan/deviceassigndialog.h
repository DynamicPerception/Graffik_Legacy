#ifndef DEVICEASSIGNDIALOG_H
#define DEVICEASSIGNDIALOG_H

#include <QDialog>
#include <QString>

#include "MoCoBus/omnetwork.h"

namespace Ui {
class DeviceAssignDialog;
}

class DeviceAssignDialog : public QDialog
{
    Q_OBJECT
    
public:
    DeviceAssignDialog(OMNetwork* c_net, QString c_bus, QString c_type, QWidget *parent = 0);
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
    int m_cmdId;
    QString m_newName;
    unsigned short m_newAddr;

    void _changeAddr(unsigned short p_old, unsigned short p_new);
    void _initInputs(QString p_type);
};

#endif // DEVICEASSIGNDIALOG_H
