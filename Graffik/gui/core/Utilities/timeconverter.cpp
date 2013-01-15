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

#include "timeconverter.h"

/** Constructor

  All methods are static.
  */

TimeConverter::TimeConverter()
{
}

/** Get Total Seconds from mS */
unsigned long TimeConverter::seconds(unsigned long p_ms) {
    return( p_ms / 1000);
}

/** Get Total Minutes from mS */
unsigned int TimeConverter::minutes(unsigned long p_ms) {
    return( p_ms / 1000 / 60);
}

/** Get Total Hours from mS */
unsigned int TimeConverter::hours(unsigned long p_ms) {
    return( p_ms / 1000 / 60 / 60);
}

unsigned int TimeConverter::days(unsigned long p_ms) {
    return(p_ms / 1000 / 60 / 60 / 24);
}

/** Get Free Hours from mS

  Free Hours are the count of hours not used by the full days in the
  specified time.  That is, if the millisecond value represents
  1 day, 3 hours, 5 minutes and 15 seconds, this method returns 3.
  */

unsigned int TimeConverter::freeHours(unsigned long p_ms) {
    unsigned int rhh = (p_ms - msFromDays(days(p_ms))) / 1000 / 60 / 60;
    return rhh;
}

/** Get Free Minutes from mS

  Free Minutes are the count of minutes not used by full hours or days
  in the specified time.  That is, if the millisecond value represents
  1 day, 3 hours, 5 minutes and 15 seconds, this method returns 5.
  */

unsigned int TimeConverter::freeMinutes(unsigned long p_ms) {
    unsigned int rmm = (p_ms - msFromHours(hours(p_ms))) / 1000 / 60;

    return rmm;
}

/** Get Free Seconds from mS

  Free Minutes are the count of seconds not used by full minutes, hours, or days
  in the specified time.  That is, if the millisecond value represents
  1 day, 3 hours, 5 minutes and 15 seconds, this method returns 15.
  */

unsigned int TimeConverter::freeSeconds(unsigned long p_ms) {
    int rss = (p_ms - msFromMinutes(minutes(p_ms))) / 1000;
    return rss;
}

/** Get Free MilliSeconds from mS

  Free milliseconds are the count of milliseconds not used by full seconds
  in the specified time.
  */

unsigned int TimeConverter::freeMilliSeconds(unsigned long p_ms) {
    int rss = p_ms - msFromSeconds(seconds(p_ms));
    return rss;
}


unsigned long TimeConverter::msFromDays(unsigned int p_days) {
    return( (unsigned long) p_days * 60 * 60 * 24 * 1000);
}

unsigned long TimeConverter::msFromHours(unsigned int p_hours) {
    return( (unsigned long) p_hours * 60 * 60 * 1000);
}

unsigned long TimeConverter::msFromMinutes(unsigned int p_minutes) {
    return( (unsigned long) p_minutes * 60 * 1000);
}

unsigned long TimeConverter::msFromSeconds(unsigned long p_seconds) {
    return( p_seconds * 1000 );
}

/** Create A String Representing Time

  Returns a QString representing the time as follows:

  HH'MM"SS.ssss

  E.g.:

  01'15"05.354

  milliseconds are represented with as many digits as needed.

  */

QString TimeConverter::stringify(unsigned long p_ms) {
    QString timeText = QString("%1").arg((unsigned int)hours(p_ms), 2, 10, QChar('0')) + "'"
            + QString("%1").arg((unsigned int)freeMinutes(p_ms), 2, 10, QChar('0')) + "\""
            + QString("%1").arg((unsigned int)freeSeconds(p_ms), 2, 10, QChar('0')) + "."
            + QString("%1").arg((unsigned int)freeMilliSeconds(p_ms));

    return timeText;
}
