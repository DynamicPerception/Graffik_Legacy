#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QDialog>

#include "MoCoBus/omnetwork.h"


#include "core/NetworkModel/networkmodel.h"
#include "core/AddNetDialog/addnetdialog.h"
#include "core/AddDeviceDialog/adddevicedialog.h"
#include "core/DeleteDeviceDialog/deletedevicedialog.h"
#include "core/DeleteBusDialog/deletebusdialog.h"
#include "core/AxisOptions/axisoptions.h"

namespace Ui {
class networkManager;
}

class networkManager : public QDialog
{
    Q_OBJECT
    
public:
    explicit networkManager(networkModel* c_net, AxisOptions* c_opts, QWidget *parent = 0);
    ~networkManager();
    
private:
    Ui::networkManager *ui;
    networkModel* _netMod;
    AxisOptions* _opts;

private slots:
    void on_addBusButton_clicked();
    void on_addDevButton_clicked();
    void on_delDevButton_clicked();
    void on_delBusButton_clicked();
    void onRowClick(QModelIndex);
};

#endif // NETWORKMANAGER_H
