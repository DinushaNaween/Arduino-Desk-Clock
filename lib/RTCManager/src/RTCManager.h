#ifndef RTC_MANAGER_H
#define RTC_MANAGER_H

#include <RTClib.h>
#include <Wire.h>
#include "../../../include/Config.h"

struct TimeData {
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
};

class RTCManager {
private:
    RTC_DS3231 rtc;  // DS3231 Precision RTC (ZS-042 module)
    bool initialized;
    bool connected;
    int timezoneOffset; // Offset in hours from UTC
    int timezoneOffsetMinutes; // Additional minutes offset (for half-hour timezones like IST +5:30)
    unsigned long startupTime; // Fallback if RTC not available
    bool lostPower; // Track if RTC lost power (battery missing/dead on ZS-042)

public:
    RTCManager();
    bool begin();
    TimeData getLocalTime();
    TimeData getUTCTime();
    void setTime(int year, int month, int day, int hour, int minute, int second);
    void setTimeFromCompileTime(); // Set time from __DATE__ and __TIME__ macros
    // Set time from a POSIX epoch (UTC). If RTC is connected, updates DS3231.
    void setTimeFromEpoch(time_t epoch);
    void setTimezone(int offsetHours) { 
        timezoneOffset = offsetHours; 
        timezoneOffsetMinutes = 0;
    }
    void setTimezone(int offsetHours, int offsetMinutes) { 
        timezoneOffset = offsetHours; 
        timezoneOffsetMinutes = offsetMinutes;
    }
    TimeData getTimeWithOffset(int offsetHours, int offsetMinutes); // Get time with specific offset
    bool isConnected() const { return connected; }
    bool hasLostPower() const { return lostPower; } // Check if RTC lost power
    DateTime now();
    float getTemperature(); // DS3231 has built-in temperature sensor
};

#endif // RTC_MANAGER_H

