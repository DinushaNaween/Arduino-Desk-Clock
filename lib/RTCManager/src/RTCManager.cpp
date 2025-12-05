#include "RTCManager.h"
#include <string.h>

RTCManager::RTCManager() : initialized(false), connected(false), timezoneOffset(0), timezoneOffsetMinutes(0), startupTime(0), lostPower(false) {
}

bool RTCManager::begin() {
    Wire.begin(I2C_SDA, I2C_SCL);
    
    // Initialize DS3231 RTC
    connected = rtc.begin();
    
    if(connected) {
        // Check if RTC lost power (OSF - Oscillator Stop Flag)
        // Note: ZS-042 should have a rechargeable LIR2032 battery installed
        // If time resets on power off, check battery installation/charge
        if(rtc.lostPower()) {
            lostPower = true;
            // RTC time is invalid, set a default time
            // User should call setTime() to set correct time
            rtc.adjust(DateTime(2024, 1, 1, 0, 0, 0));
        } else {
            lostPower = false;
        }
    } else {
        // Store startup time as fallback if RTC not found
        startupTime = millis() / 1000;
        lostPower = true;
    }
    
    initialized = true;
    return true; // Return true even if RTC not connected (we have fallback)
}

DateTime RTCManager::now() {
    if(!connected) {
        // Calculate time from startup time
        unsigned long elapsed = (millis() / 1000) - startupTime;
        unsigned long totalSeconds = startupTime + elapsed;
        int hours = (totalSeconds / 3600) % 24;
        int minutes = (totalSeconds / 60) % 60;
        int seconds = totalSeconds % 60;
        int days = totalSeconds / 86400;
        // Simple date calculation (starting from 2024-01-01)
        return DateTime(2024, 1, 1 + days, hours, minutes, seconds);
    }
    return rtc.now();
}

TimeData RTCManager::getUTCTime() {
    DateTime dt = now();
    TimeData time;
    time.year = dt.year();
    time.month = dt.month();
    time.day = dt.day();
    time.hour = dt.hour();
    time.minute = dt.minute();
    time.second = dt.second();
    return time;
}

TimeData RTCManager::getLocalTime() {
    DateTime dt = now();
    // Apply timezone offset (hours and minutes)
    dt = dt + TimeSpan(0, timezoneOffset, timezoneOffsetMinutes, 0);
    
    TimeData time;
    time.year = dt.year();
    time.month = dt.month();
    time.day = dt.day();
    time.hour = dt.hour();
    time.minute = dt.minute();
    time.second = dt.second();
    return time;
}

TimeData RTCManager::getTimeWithOffset(int offsetHours, int offsetMinutes) {
    DateTime dt = now();
    // Apply specific timezone offset
    dt = dt + TimeSpan(0, offsetHours, offsetMinutes, 0);
    
    TimeData time;
    time.year = dt.year();
    time.month = dt.month();
    time.day = dt.day();
    time.hour = dt.hour();
    time.minute = dt.minute();
    time.second = dt.second();
    return time;
}

void RTCManager::setTime(int year, int month, int day, int hour, int minute, int second) {
    if(connected) {
        rtc.adjust(DateTime(year, month, day, hour, minute, second));
        lostPower = false; // Time is now set, clear lost power flag
    }
}

float RTCManager::getTemperature() {
    if(connected) {
        // DS3231 has a built-in temperature sensor
        return rtc.getTemperature();
    }
    return 0.0; // Return 0 if not connected
}

void RTCManager::setTimeFromCompileTime() {
    // Parse __DATE__ macro (format: "Nov 22 2024" or "Nov  2 2024")
    // Parse __TIME__ macro (format: "14:30:45")
    
    const char* dateStr = __DATE__;
    const char* timeStr = __TIME__;
    
    // Month names
    const char* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                            "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    
    char monthStr[4];
    int day, year;
    int hour, minute, second;
    
    // Parse date: "Nov 22 2024" or "Nov  2 2024" (note: space before single digit)
    sscanf(dateStr, "%s %d %d", monthStr, &day, &year);
    
    // Find month number
    int month = 1;
    for(int i = 0; i < 12; i++) {
        if(strcmp(monthStr, months[i]) == 0) {
            month = i + 1;
            break;
        }
    }
    
    // Parse time: "14:30:45"
    sscanf(timeStr, "%d:%d:%d", &hour, &minute, &second);
    
    // Set the RTC time
    setTime(year, month, day, hour, minute, second);
}

void RTCManager::setTimeFromEpoch(time_t epoch) {
    struct tm tm;
#if defined(_MSC_VER)
    gmtime_s(&tm, &epoch);
#else
    gmtime_r(&epoch, &tm);
#endif
    int year = tm.tm_year + 1900;
    int month = tm.tm_mon + 1;
    int day = tm.tm_mday;
    int hour = tm.tm_hour;
    int minute = tm.tm_min;
    int second = tm.tm_sec;
    setTime(year, month, day, hour, minute, second);
}

