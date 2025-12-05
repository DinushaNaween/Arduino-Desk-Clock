#include "SensorManager.h"

SensorManager::SensorManager() 
    : initialized(false), connected(false), 
      lastTemperature(0.0), lastHumidity(0.0),
      lastUpdate(0), updateInterval(SENSOR_UPDATE_INTERVAL) {
}

bool SensorManager::begin() {
    Wire.begin(I2C_SDA, I2C_SCL);
    connected = aht.begin();
    initialized = true;
    return connected;
}

bool SensorManager::update() {
    if(!initialized || !connected) {
        return false;
    }
    
    unsigned long now = millis();
    if(now - lastUpdate < updateInterval) {
        return true; // Use cached values
    }
    
    sensors_event_t humidity, temp;
    if(aht.getEvent(&humidity, &temp)) {
        lastTemperature = temp.temperature;
        lastHumidity = humidity.relative_humidity;
        lastUpdate = now;
        return true;
    }
    
    return false;
}

float SensorManager::readTemperature() {
    update(); // Ensure data is fresh
    return lastTemperature;
}

float SensorManager::readHumidity() {
    update(); // Ensure data is fresh
    return lastHumidity;
}

