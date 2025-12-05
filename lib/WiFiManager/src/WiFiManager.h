#pragma once

#include <Arduino.h>
#include <time.h>

class WiFiManagerClass {
public:
    WiFiManagerClass();
    // Connect to Wi-Fi with timeout (ms). Returns true if connected.
    bool begin(const char* ssid, const char* password, unsigned long timeoutMs = 15000);
    // Returns whether currently connected
    bool isConnected();
    // Sync time via NTP and configure TZ. tz is IANA string like "Asia/Kolkata".
    // Returns true if time successfully obtained.
    bool syncTime(const char* tz, unsigned long timeoutMs = 10000);
    // Convenience: return time_t now (0 if not available)
    time_t getEpoch();
};

extern WiFiManagerClass WiFiManager;
