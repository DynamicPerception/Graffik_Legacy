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

#ifndef FILMFILEHANDLER_H
#define FILMFILEHANDLER_H

#include <QFile>
#include <QString>
#include <QApplication>

#include "MoCoBus/omnetwork.h"

#include "film/FilmParameters/filmparameters.h"

#include "core/Dialogs/errordialog.h"

#include "addressnotfounddialog.h"

/** Film File Handler Class

  This class handles reading and writing a film object from/to
  disk.

  @author
  C. A. Church
  */

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
