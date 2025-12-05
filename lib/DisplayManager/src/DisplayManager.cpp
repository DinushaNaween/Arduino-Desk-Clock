#include "DisplayManager.h"

DisplayManager::DisplayManager() : initialized(false) {
    // Initialize display objects with pin configurations
    displays[0] = new Adafruit_GC9A01A(DISP1_CS, DISP1_DC, DISP1_RST);
    displays[1] = new Adafruit_GC9A01A(DISP2_CS, DISP2_DC, DISP2_RST);
    displays[2] = new Adafruit_GC9A01A(DISP3_CS, DISP3_DC, DISP3_RST);
    displays[3] = new Adafruit_GC9A01A(DISP4_CS, DISP4_DC, DISP4_RST);
}

DisplayManager::~DisplayManager() {
    for (int i = 0; i < NUM_DISPLAYS; ++i) {
        if (displays[i]) {
            delete displays[i];
            displays[i] = nullptr;
        }
    }
}

bool DisplayManager::begin() {
    // Initialize SPI bus
    SPI.begin(PIN_SCLK, -1, PIN_MOSI, -1);
    
    // Initialize all displays
    for(int i = 0; i < NUM_DISPLAYS; i++) {
        if (displays[i]) {
            displays[i]->begin();
            displays[i]->fillScreen(0x0000); // Clear screen
        }
    }
    
    initialized = true;
    return true;
}

Adafruit_GC9A01A& DisplayManager::getDisplay(int index) {
    if(index >= 0 && index < NUM_DISPLAYS) {
        return *displays[index];
    }
    return *displays[0]; // Return first display as fallback
}

void DisplayManager::clearAll() {
    for(int i = 0; i < NUM_DISPLAYS; i++) {
        if (displays[i]) {
            displays[i]->fillScreen(0x0000);
        }
    }
}

