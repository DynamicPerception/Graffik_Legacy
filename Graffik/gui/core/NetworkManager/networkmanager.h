#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QDialog>

#include "MoCoBus/omnetwork.h"


#include "core/NetworkModel/networkmodel.h"
#include "core/Dialogs/addnetdialog.h"
#include "core/Dialogs/adddevicedialog.h"
#include "core/Dialogs/deletedevicedialog.h"
#include "core/Dialogs/deletebusdialog.h"
#include "core/Options/axisoptions.h"

namespace Ui {
class NetworkManager;
}

class NetworkManager : public QDialog
{
    Q_OBJECT
    
public:
    NetworkManager(NetworkModel* c_net, AxisOptions* c_opts, QWidget *parent = 0);
    ~NetworkManager();
    
private:
    Ui::NetworkManager *ui;
    NetworkModel* _netMod;
    AxisOptions* _opts;

private slots:
    void on_addBusButton_clicked();
    void on_addDevButton_clicked();
    void on_delDevButton_clicked();
    void on_delBusButton_clicked();
    void onRowClick(QModelIndex);
};

#endif // NETWORKMANAGER_H
