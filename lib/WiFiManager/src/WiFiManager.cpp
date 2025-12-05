#include "WiFiManager.h"
#include <WiFi.h>

WiFiManagerClass WiFiManager;

WiFiManagerClass::WiFiManagerClass() {
}

bool WiFiManagerClass::begin(const char* ssid, const char* password, unsigned long timeoutMs) {
    if(WiFi.status() == WL_CONNECTED) return true;
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && (millis() - start) < timeoutMs) {
        delay(200);
    }
    return WiFi.status() == WL_CONNECTED;
}

bool WiFiManagerClass::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}

bool WiFiManagerClass::syncTime(const char* tz, unsigned long timeoutMs) {
    if(!isConnected()) return false;
    // Use upstream NTP servers and set TZ
    configTzTime(tz, "pool.ntp.org", "time.nist.gov");
    struct tm timeinfo;
    unsigned long start = millis();
    while(!getLocalTime(&timeinfo) && (millis() - start) < timeoutMs) {
        delay(200);
    }
    return getLocalTime(&timeinfo);
}

time_t WiFiManagerClass::getEpoch() {
    // Return UTC epoch (system time() is always UTC regardless of TZ)
    time_t now = time(nullptr);
    if(now <= 100000) return 0;
    return now;
}
