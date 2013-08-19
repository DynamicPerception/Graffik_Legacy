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

#ifndef OPTIONTYPES_H
#define OPTIONTYPES_H

/** Available Option Types */
namespace Options {

    const int Imperial = 1;
    const int   Metric = 2;
    const int    Steps = 3;
}

/** Error Types */

namespace AxisErrors {

        /** Maximum speed exceeded for axis */
    const int ErrorSpeedExceeded = 1;
        /** Not enough free interval time */
    const int    ErrorNoInterval = 2;
        /** Not enough total free interval time during film */
    const int        ErrorNoTime = 3;
        /** At least one interval needs more time than allocated based on move */
    const int ErrorIntervalSpeed = 4;
}

namespace HotKeys {

    /** Film Play */
    const int FilmPlay = 1;
    /** Film Stop */
    const int FilmStop = 2;
    /** Film Fwd to End */
    const int  FilmFwd = 3;
    /** Film Rwd to Beginning */
    const int  FilmRwd = 4;
    /** Film Frame Advance */
    const int FilmFFwd = 5;
    /** Film Frame Reverse */
    const int FilmFRwd = 6;
}
#endif // OPTIONTYPES_H
