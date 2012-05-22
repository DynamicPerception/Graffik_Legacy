#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QDialog>

#include "MoCoBus/omnetwork.h"


#include "core/NetworkModel/networkmodel.h"
#include "core/AddNetDialog/addnetdialog.h"
#include "core/AddDeviceDialog/adddevicedialog.h"

#include "film/OMAxisFilmOptions/omaxisfilmoptions.h"

namespace Ui {
class networkManager;
}

class networkManager : public QDialog
{
    Q_OBJECT
    
public:
    explicit networkManager(networkModel* c_net, OMAxisFilmOptions* c_opts, QWidget *parent = 0);
    ~networkManager();
    
private:
    Ui::networkManager *ui;
    networkModel* _netMod;
    OMAxisFilmOptions* _opts;

private slots:
    void on_addBusButton_clicked();
    void on_addDevButton_clicked();
    void onRowClick(QModelIndex);
};

#endif // NETWORKMANAGER_H
