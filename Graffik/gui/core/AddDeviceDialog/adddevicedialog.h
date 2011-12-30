#ifndef ADDDEVICEDIALOG_H
#define ADDDEVICEDIALOG_H

#include <QDialog>
#include <QStringList>

#include "MoCoBus/omnetwork.h"
#include "Slim/slimcommandparser.h"

namespace Ui {
    class addDeviceDialog;
}

class addDeviceDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit addDeviceDialog(OMNetwork*, QWidget *parent = 0);
    addDeviceDialog(OMNetwork*, SlimCommandParser*, QWidget *parent = 0);
    ~addDeviceDialog();
    
    void updateBuses();

public slots:
    void accept();

private:
    Ui::addDeviceDialog *ui;
    OMNetwork *_net;
    SlimCommandParser *_parser;

    void _initInputs();

};

#endif // ADDDEVICEDIALOG_H
