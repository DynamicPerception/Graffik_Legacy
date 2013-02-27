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


#ifndef FILMTIMEMANAGER_H
#define FILMTIMEMANAGER_H

#include <QObject>
#include "film/FilmParameters/filmparameters.h"
#include "filmexec.h"

/** Film Time Manager

  The film time manager class provides a central interface to noting current film time,
  seeking to specific times, and broadcasting time changes.

  @author
  C. A. Church
  */

class FilmTimeManager : public QObject
{
    Q_OBJECT
public:
    FilmTimeManager(FilmExec* c_exec, FilmParameters* c_params, QObject *parent = 0);
    ~FilmTimeManager();
    
    void timePosition(unsigned long p_time);
    unsigned long timePosition();

signals:
    
     /** Time Position Changed Signal

       This signal is emitted whenever a time position change is received
       */
    void timePositionChanged(unsigned long p_time);

public slots:
    void filmParamsChanged();
    void playStatus(bool p_runStat, unsigned long p_curTime);

private:

    FilmParameters* m_params;
    FilmExec* m_exec;
    unsigned long m_filmTime;
    unsigned long m_curTime;

};

#endif // FILMTIMEMANAGER_H
