#include "PowerMonitor.h"

PowerMonitor::PowerMonitor() 
    : initialized(false), connected(false),
      lastVoltage(0.0), lastCurrent(0.0), lastPower(0.0),
      lastUpdate(0), updateInterval(SENSOR_UPDATE_INTERVAL) {
}

bool PowerMonitor::begin() {
    Wire.begin(I2C_SDA, I2C_SCL);
    connected = ina.begin();
    if(connected) {
        // Optional: Set calibration for 32V, 2A range
        // ina.setCalibration_32V_2A();
    }
    initialized = true;
    return connected;
}

bool PowerMonitor::update() {
    if(!initialized || !connected) {
        return false;
    }
    
    unsigned long now = millis();
    if(now - lastUpdate < updateInterval) {
        return true; // Use cached values
    }
    
    lastVoltage = ina.getBusVoltage_V();
    lastCurrent = ina.getCurrent_mA();
    lastPower = ina.getPower_mW();
    lastUpdate = now;
    
    return true;
}

float PowerMonitor::getVoltage() {
    update(); // Ensure data is fresh
    return lastVoltage;
}

float PowerMonitor::getCurrent() {
    update(); // Ensure data is fresh
    return lastCurrent;
}

float PowerMonitor::getPower() {
    update(); // Ensure data is fresh
    return lastPower;
}

