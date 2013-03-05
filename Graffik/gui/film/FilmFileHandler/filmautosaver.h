/*

  Graffik Motion Control Application

  Copyright (c) 2011-2013 Dynamic Perception

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

#ifndef FILMAUTOSAVER_H
#define FILMAUTOSAVER_H

#include <QObject>
#include <QFile>
#include <QDataStream>

#include "filmfilehandler.h"
#include "MoCoBus/omnetwork.h"
#include "film/FilmParameters/filmparameters.h"

const QString  FAS_AUTO_FILE = ".film_autosave";
const QString FAS_STATE_FILE = ".film.state";

/** Film Auto Saver

  Automatically saves a copy of the current working film as it changes.  Used for saving
  the current film state and restoring it on program start.

  @author
  C. A. Church
  */

class FilmAutoSaver : public QObject
{
    Q_OBJECT
public:
    explicit FilmAutoSaver(OMNetwork* c_net, FilmParameters* c_parms, QObject *parent = 0);
    
    bool savedStatus();
    void loadSavedFilm();

signals:
    

public slots:
    
    void filmParamsReleased();
    void playStatusChanged(bool p_state);

private:
    OMNetwork* m_net;
    FilmParameters* m_parms;
    QString m_filmPath;
    QString m_statPath;
    bool m_curState;

};

#endif // FILMAUTOSAVER_H
