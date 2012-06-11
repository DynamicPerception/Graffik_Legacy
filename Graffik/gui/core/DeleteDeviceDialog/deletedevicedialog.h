#ifndef DELETEDEVICEDIALOG_H
#define DELETEDEVICEDIALOG_H

#include <QDialog>
#include <QList>
#include <QHash>

#include "MoCoBus/omnetwork.h"

namespace Ui {
class DeleteDeviceDialog;
}

class DeleteDeviceDialog : public QDialog
{
    Q_OBJECT
    
public:
    DeleteDeviceDialog(OMNetwork *c_net, QWidget *parent = 0);
    ~DeleteDeviceDialog();
    
private slots:
    void on_okButton_clicked();
    void on_cancelButton_clicked();

private:
    Ui::DeleteDeviceDialog *ui;
    OMNetwork* m_net;
    QList< QHash<unsigned short, QString>* > m_devList;
};

#endif // DELETEDEVICEDIALOG_H
