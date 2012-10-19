#ifndef ADDDEVICEDIALOG_H
#define ADDDEVICEDIALOG_H

#include <QDialog>
#include <QStringList>

#include "MoCoBus/omnetwork.h"


namespace Ui {
    class AddDeviceDialog;
}

class AddDeviceDialog : public QDialog
{
    Q_OBJECT
    
public:
    AddDeviceDialog(OMNetwork* c_net, QWidget *parent = 0);
    AddDeviceDialog(OMNetwork* c_net, QString c_bus, QString c_type, unsigned short c_addr, QString c_name, QWidget *parent = 0);

    ~AddDeviceDialog();
    
    void updateBuses();

public slots:
    void accept();

private:
    Ui::AddDeviceDialog *ui;
    OMNetwork *_net;

    void _initInputs();

};

#endif // ADDDEVICEDIALOG_H
