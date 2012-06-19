#ifndef DELETEBUSDIALOG_H
#define DELETEBUSDIALOG_H

#include <QDialog>
#include <QList>
#include <QString>

#include "MoCoBus/omnetwork.h"

namespace Ui {
class DeleteBusDialog;
}

class DeleteBusDialog : public QDialog
{
    Q_OBJECT
    
public:
    DeleteBusDialog(OMNetwork* c_net, QWidget *parent = 0);
    ~DeleteBusDialog();
    
private slots:
    void on_okButton_clicked();
    void on_cancelButton_clicked();

private:
    Ui::DeleteBusDialog *ui;
    OMNetwork* m_net;
    QList<QString> m_buses;
};

#endif // DELETEBUSDIALOG_H
