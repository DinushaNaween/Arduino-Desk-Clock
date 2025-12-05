#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>
#include "../../../include/Config.h"

class DisplayManager {
private:
    Adafruit_GC9A01A* displays[NUM_DISPLAYS];
    bool initialized;

public:
    DisplayManager();
    ~DisplayManager();
    DisplayManager(const DisplayManager&) = delete;
    DisplayManager& operator=(const DisplayManager&) = delete;
    bool begin();
    Adafruit_GC9A01A& getDisplay(int index);
    void clearAll();
    bool isInitialized() const { return initialized; }
};

#endif // DISPLAY_MANAGER_H

