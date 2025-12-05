#pragma once

#include <Arduino.h>
#include <time.h>

struct WeatherData {
    String city;
    float tempC = 0.0f;
    String condition;
    unsigned long lastUpdate = 0;
    bool valid = false;
};

class WeatherClientClass {
public:
    WeatherClientClass();
    // Call periodically to refresh if needed. Returns true if a fresh fetch occurred.
    bool update(unsigned long minIntervalMs = 600000); // default 10 min
    // Get latest cached data
    const WeatherData& get() const;
    void begin();
private:
    WeatherData data;
    unsigned long lastFetch = 0;
    bool fetch();
};

extern WeatherClientClass WeatherClient;
