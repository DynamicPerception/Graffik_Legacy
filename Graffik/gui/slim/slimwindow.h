/*

  Graffik Motion Control Application

  Copyright (c) 2011-2012 Dynamic Perception

 This file is part of Graffik.

    Graffik is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Graffik is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Graffik.  If not, see <http://www.gnu.org/licenses/>.

    */

#ifndef SLIMWINDOW_H
#define SLIMWINDOW_H

#include <QWidget>
#include <QString>
#include <QFileDialog>


#include "core/Dialogs/errordialog.h"

#include "MoCoBus/omcommandbuffer.h"
#include "MoCoBus/omnetwork.h"
#include "Slim/slimcommandparser.h"

#include "slim/SlimFileHandler/slimfilehandler.h"
#include "slim/CommandHistoryModel/commandhistorymodel.h"

namespace Ui {
class SlimWindow;
}

/** Slim Window Widget

  The Slim Window Widget generates the base UI for the scripting section of Graffik

  @author
  C. A. Church
  */

class SlimWindow : public QWidget
{
    Q_OBJECT

public:
    SlimWindow(OMNetwork* net, CommandHistoryModel* hist, SlimCommandParser* parse, QWidget *parent = 0);
    ~SlimWindow();

private slots:
    void onCmdEntry();

public slots:
        void registerNewDevice(OMbusInfo* p_bus, OMdeviceInfo* p_dev);
        void removeDevice(OMbusInfo* p_bus, unsigned short p_addr);
        void save();
        void load();

private:

    Ui::SlimWindow *ui;
    CommandHistoryModel* _cmdHist;
    OMNetwork* _net;
    SlimCommandParser * _parser;
};


#endif // SLIMWINDOW_H
