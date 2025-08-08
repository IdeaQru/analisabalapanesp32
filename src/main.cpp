#include "RacingTelemetry.h"
// pembaruan hari ini
RacingTelemetry* racingSystem = nullptr;

void setup() {
    Serial.begin(230400);
    Serial.println("=== ESP32 Racing Telemetry System ===");
    Serial.println("Initializing OOP-based system...");
    
    // Create and initialize the main system
    racingSystem = &RacingTelemetry::getInstance();
    racingSystem->initialize();
    
    Serial.println("System ready!");
}

void loop() {
    if (racingSystem) {

        racingSystem->handleSerialInput();
        racingSystem->update();
    }
    
    yield();
}
