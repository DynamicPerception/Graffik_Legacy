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

#ifndef TIMECONVERTER_H
#define TIMECONVERTER_H

#include <QString>

/** Time Conversion Utilities

  Provides basic time conversion functions (milliseconds to minutes, hours, etc.)

  All methods are static

  @author
  C. A. Church
  */

class TimeConverter {

public:
    TimeConverter();

    static unsigned int days(unsigned long p_ms);
    static unsigned int hours(unsigned long p_ms);
    static unsigned int minutes(unsigned long p_ms);
    static unsigned long seconds(unsigned long p_ms);

    static unsigned int freeHours(unsigned long p_ms);
    static unsigned int freeMinutes(unsigned long p_ms);
    static unsigned int freeSeconds(unsigned long p_ms);
    static unsigned int freeMilliSeconds(unsigned long p_ms);

    static unsigned long msFromSeconds(float p_seconds);
    static unsigned long msFromDays(float p_days);
    static unsigned long msFromHours(float p_hours);
    static unsigned long msFromMinutes(float p_minutes);

    static QString stringify(unsigned long p_ms);

};

#endif // TIMECONVERTER_H
