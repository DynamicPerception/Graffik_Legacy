#ifndef DELETEDEVICEDIALOG_H
#define DELETEDEVICEDIALOG_H

#include <QDialog>

#include "MoCoBus/omnetwork.h"

namespace Ui {
class DeleteDeviceDialog;
}

class DeleteDeviceDialog : public QDialog
{
    Q_OBJECT
    
public:
    DeleteDeviceDialog(QWidget *parent = 0);
    ~DeleteDeviceDialog();
    
private:
    Ui::DeleteDeviceDialog *ui;
};

#endif // DELETEDEVICEDIALOG_H
