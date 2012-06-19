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
    AddDeviceDialog(OMNetwork*, QWidget *parent = 0);
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
