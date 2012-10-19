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

#ifndef FILMGLOBALOPTIONS_H
#define FILMGLOBALOPTIONS_H

/** Film Global Options Class

  @author
  C. A. Church
  */

class FilmGlobalOptions
{
public:
    FilmGlobalOptions();
    ~FilmGlobalOptions();

    void metric(bool p_metric);
    bool isMetric();


private:
    bool m_metric;
};

#endif // FILMGLOBALOPTIONS_H
