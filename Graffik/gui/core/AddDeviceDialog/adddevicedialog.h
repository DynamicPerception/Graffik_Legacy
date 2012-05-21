#ifndef ADDDEVICEDIALOG_H
#define ADDDEVICEDIALOG_H

#include <QDialog>
#include <QStringList>

#include "MoCoBus/omnetwork.h"


namespace Ui {
    class addDeviceDialog;
}

class addDeviceDialog : public QDialog
{
    Q_OBJECT
    
public:
    addDeviceDialog(OMNetwork*, QWidget *parent = 0);
    ~addDeviceDialog();
    
    void updateBuses();

public slots:
    void accept();

private:
    Ui::addDeviceDialog *ui;
    OMNetwork *_net;

    void _initInputs();

};

#endif // ADDDEVICEDIALOG_H
