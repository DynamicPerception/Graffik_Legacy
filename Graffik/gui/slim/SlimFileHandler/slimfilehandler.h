#ifndef SLIMFILEHANDLER_H
#define SLIMFILEHANDLER_H


#include "Slim/slimcommandparser.h"
#include "Slim/commandhistorymodel.h"
#include "MoCoBus/omnetwork.h"

/** This class handles reading and writing Slim script files
  */

class SlimFileHandler
{
public:
    SlimFileHandler(SlimCommandParser* c_parse, CommandHistoryModel* c_hist);
    ~SlimFileHandler();

    static void writeFile(QString p_file, OMNetwork* p_net, CommandHistoryModel* p_hist, bool p_showErr = true);
    static void readFile(QString p_file, SlimCommandParser* p_parse, CommandHistoryModel* p_hist, bool p_showErr = true);

private:

    static void _trimLeadingSpaces(QString &line);

};

#endif // SLIMFILEHANDLER_H
