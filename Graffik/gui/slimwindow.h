#ifndef SLIMWINDOW_H
#define SLIMWINDOW_H

#include <QWidget>
#include <QString>
#include <QCloseEvent>


#include "Core/SpeedDial/dialspeed.h"
#include "Core/NetworkStatusDisplay/networkstatusgrid.h"

#include "MoCoBus/omcommandbuffer.h"
#include "MoCoBus/omnetwork.h"
#include "Slim/commandhistorymodel.h"
#include "Slim/slimcommandparser.h"

namespace Ui {
class SlimWindow;
}

class SlimWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SlimWindow(OMNetwork* net, SlimCommandParser* parse, QWidget *parent = 0);
    ~SlimWindow();

private slots:
    void onCmdEntry();
    void onDialActivate();
    void onCmdResult(slimCommand);

public slots:
        void registerNewDevice(OMbusInfo* p_bus, OMdeviceInfo* p_dev);
private:

    Ui::SlimWindow *ui;

    OMNetwork* _net;
    CommandHistoryModel* _cmdHist;
    SlimCommandParser * _parser;
    NetworkStatusGrid* _netState;
};


#endif // MAINWINDOW_H
