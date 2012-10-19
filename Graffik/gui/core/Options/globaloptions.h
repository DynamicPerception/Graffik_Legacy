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

#ifndef GLOBALOPTIONS_H
#define GLOBALOPTIONS_H

#include <QObject>
#include <QMutex>


#include "optiontypes.h"

/** Global Options Class

  This class manages a set of global options available to the user interface.

  All set and get methods are thread-safe.

  @author
  C. A. Church
  */

class GlobalOptions : public QObject
{
    Q_OBJECT
public:
    explicit GlobalOptions(QObject *parent = 0);
    ~GlobalOptions();

    bool stopOnErr();
    void stopOnErr(bool p_mode);

    int display();
    void display(int p_mode);
    
signals:
    void optionsChanged();

public slots:
    
private:
    bool m_stopErr;
    int m_display;

    QMutex* m_mutex;
};

#endif // GLOBALOPTIONS_H
