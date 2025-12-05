#ifndef POWER_MONITOR_H
#define POWER_MONITOR_H

#include <Adafruit_INA219.h>
#include <Wire.h>
#include "../../../include/Config.h"

class PowerMonitor {
private:
    Adafruit_INA219 ina;
    bool initialized;
    bool connected;
    float lastVoltage;
    float lastCurrent;
    float lastPower;
    unsigned long lastUpdate;
    unsigned long updateInterval;

public:
    PowerMonitor();
    bool begin();
    bool update();
    float getVoltage();
    float getCurrent();
    float getPower();
    bool isConnected() const { return connected; }
    void setUpdateInterval(unsigned long interval) { updateInterval = interval; }
};

#endif // POWER_MONITOR_H

