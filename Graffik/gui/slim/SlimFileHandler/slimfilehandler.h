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
