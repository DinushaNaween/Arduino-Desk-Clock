#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <Adafruit_AHTX0.h>
#include <Wire.h>
#include "../../../include/Config.h"

class SensorManager {
private:
    Adafruit_AHTX0 aht;
    bool initialized;
    bool connected;
    float lastTemperature;
    float lastHumidity;
    unsigned long lastUpdate;
    unsigned long updateInterval;

public:
    SensorManager();
    bool begin();
    bool update();
    float readTemperature();
    float readHumidity();
    bool isConnected() const { return connected; }
    void setUpdateInterval(unsigned long interval) { updateInterval = interval; }
};

#endif // SENSOR_MANAGER_H

