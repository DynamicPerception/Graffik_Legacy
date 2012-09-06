#ifndef NETBASEDIALOG_H
#define NETBASEDIALOG_H

#include <QDialog>

#include "MoCoBus/omnetwork.h"

#include "core/DeviceScan/devicescanner.h"
#include "core/Options/axisoptions.h"

#include "core/NetworkManager/networkmanager.h"
#include "core/NetworkModel/networkmodel.h"

namespace Ui {
class NetBaseDialog;
}

class NetBaseDialog : public QDialog
{
    Q_OBJECT
    
public:
    NetBaseDialog(OMNetwork* c_net, NetworkModel* c_model, AxisOptions* c_opts, QWidget *parent = 0);
    ~NetBaseDialog();
    
public slots:
    void on_scanButton_clicked();
    void on_initButton_clicked();
    void on_manageButton_clicked();

private:
    Ui::NetBaseDialog *ui;
    OMNetwork* m_net;

    DeviceScanner* m_scan;
    AxisOptions* m_opts;
    NetworkModel* m_model;
};

#endif // NETBASEDIALOG_H
