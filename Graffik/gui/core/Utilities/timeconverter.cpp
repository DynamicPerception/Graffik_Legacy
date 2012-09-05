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
