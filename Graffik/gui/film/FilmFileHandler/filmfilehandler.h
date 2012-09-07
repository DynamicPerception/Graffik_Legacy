#ifndef FILMFILEHANDLER_H
#define FILMFILEHANDLER_H

#include <QFile>
#include <QString>

#include "film/FilmParameters/filmparameters.h"
#include "core/Dialogs/errordialog.h"

class FilmFileHandler
{
public:
    FilmFileHandler();
    ~FilmFileHandler();

    static void writeFile(QString p_file, FilmParameters* p_params, bool p_showErr = true);
    static void readFile(QString p_file, FilmParameters* p_params, bool p_showErr = true);

};

#endif // FILMFILEHANDLER_H
