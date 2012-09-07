#ifndef FILMFILEHANDLER_H
#define FILMFILEHANDLER_H

#include <QFile>
#include <QString>

#include "MoCoBus/omnetwork.h"

#include "film/FilmParameters/filmparameters.h"

#include "core/Dialogs/errordialog.h"

#include "addressnotfounddialog.h"

class FilmFileHandler
{
public:
    FilmFileHandler();
    ~FilmFileHandler();

    static void writeFile(QString p_file, FilmParameters* p_params, bool p_showErr = true);
    static void readFile(QString p_file, FilmParameters* p_params, OMNetwork* p_net, bool p_showErr = true);

private:

    static void _validateDevices(OMfilmParams *p_current, OMfilmParams *p_new, OMNetwork *p_net);
};

#endif // FILMFILEHANDLER_H
