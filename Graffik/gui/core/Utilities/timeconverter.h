#ifndef TIMECONVERTER_H
#define TIMECONVERTER_H

/** Time Conversion Utilities

  Provides basic time conversion functions (milliseconds to minutes, hours, etc.)
  */

class TimeConverter
{
public:
    TimeConverter();

    static unsigned int days(unsigned long p_ms);
    static unsigned int hours(unsigned long p_ms);
    static unsigned int minutes(unsigned long p_ms);
    static unsigned long seconds(unsigned long p_ms);

    static unsigned int freeHours(unsigned long p_ms);
    static unsigned int freeMinutes(unsigned long p_ms);
    static unsigned int freeSeconds(unsigned long p_ms);

    static unsigned long msFromSeconds(unsigned long p_seconds);
    static unsigned long msFromDays(unsigned int p_days);
    static unsigned long msFromHours(unsigned int p_hours);
    static unsigned long msFromMinutes(unsigned int p_minutes);

};

#endif // TIMECONVERTER_H
