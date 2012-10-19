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

#ifndef SLIMFILEHANDLER_H
#define SLIMFILEHANDLER_H

#include <QFile>
#include <QTextStream>


#include "core/Dialogs/errordialog.h"

#include "Slim/slimcommandparser.h"
#include "MoCoBus/omnetwork.h"

#include "slim/CommandHistoryModel/commandhistorymodel.h"

/** This class handles reading and writing Slim script files to disk.

  @author C. A. Church

  */

class SlimFileHandler
{
public:
    SlimFileHandler();
    ~SlimFileHandler();

    static void writeFile(QString p_file, CommandHistoryModel *p_hist, bool p_showErr = true);
    static void readFile(QString p_file, SlimCommandParser* p_parse, CommandHistoryModel* p_hist, bool p_showErr = true);

private:

    static void _trimLeadingSpaces(QString &line);

};

#endif // SLIMFILEHANDLER_H
