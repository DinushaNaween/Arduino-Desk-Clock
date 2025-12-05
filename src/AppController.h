#ifndef APP_CONTROLLER_H
#define APP_CONTROLLER_H

#include "../lib/DisplayManager/src/DisplayManager.h"
#include "../lib/ClockDisplay/src/ClockDisplay.h"
#include "../lib/SensorManager/src/SensorManager.h"
#include "../lib/PowerMonitor/src/PowerMonitor.h"
#include "../lib/RTCManager/src/RTCManager.h"
#include "../lib/BuzzerController/src/BuzzerController.h"
#include "../lib/Screensaver/src/Screensaver.h"
#include "../include/Config.h"

class AppController {
private:
    DisplayManager displayManager;
    ClockDisplay* localClock;
    ClockDisplay* utcClock;
    SensorManager sensorManager;
    PowerMonitor powerMonitor;
    RTCManager rtcManager;
    BuzzerController buzzer;
    Screensaver* screensaver;
    bool tempAlertActive;
    
    bool initialized;
    unsigned long lastSensorUpdate;
    unsigned long lastDisplayUpdate;
    
    void updateSensorDisplay();
    void drawSensorData(float temp, float hum, float voltage, float current);
    void drawWeatherData();

public:
    AppController();
    bool begin();
    void update();
    void updateDisplays();
    void updateSensors();
};

#endif // APP_CONTROLLER_H

