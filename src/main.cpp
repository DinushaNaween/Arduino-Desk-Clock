#include "AppController.h"

AppController app;

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("Initializing Desk Clock...");
    
    if(!app.begin()) {
        Serial.println("Failed to initialize application!");
        while(1) delay(1000);
    }
    
    Serial.println("Desk Clock initialized successfully!");
}

void loop() {
    app.update();
    delay(10); // Small delay to prevent watchdog issues
}
