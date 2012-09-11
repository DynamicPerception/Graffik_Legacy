#ifndef ADDNETDIALOG_H
#define ADDNETDIALOG_H

#include <QDialog>

#include "MoCoBus/omnetwork.h"

namespace Ui {
    class AddNetDialog;
}

class AddNetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddNetDialog(OMNetwork*, QWidget *parent = 0);
    ~AddNetDialog();

    void updateSerialPorts();

private:
    Ui::AddNetDialog *ui;
    OMNetwork *_net;
    QWidget* _parent;


public slots:
    void accept();


private slots:
    void on_rescanButton_clicked();
    void on_portCombo_currentIndexChanged(int);

};

#endif // ADDNETDIALOG_H
