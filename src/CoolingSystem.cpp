#include "CoolingSystem.h"

CoolingSystem::CoolingSystem() 
    : ewpStatus(false), fanStatus(false), cutoffStatus(false),
      fanOnTemp(Config::DEFAULT_FAN_TEMP), cutoffTemp(Config::DEFAULT_CUTOFF_TEMP),
      lastUpdate(0), currentTemp(0.0f), systemActive(false) {
}

CoolingSystem::~CoolingSystem() {
    stop();
}

void CoolingSystem::initialize() {
    pinMode(Config::PIN_EWP, OUTPUT);
    pinMode(Config::PIN_FAN, OUTPUT);
    pinMode(Config::PIN_CUTOFF, OUTPUT);
    
    digitalWrite(Config::PIN_EWP, LOW);
    digitalWrite(Config::PIN_FAN, LOW);
    digitalWrite(Config::PIN_CUTOFF, LOW);
    
    ewpStatus = false;
    fanStatus = false;
    cutoffStatus = false;
    systemActive = false;
    
    Serial.println("=== Cooling System Initialized ===");
    Serial.printf("EWP Pin: %d (auto ON when system active)\n", Config::PIN_EWP);
    Serial.printf("Fan Pin: %d (ON when temp >= %.0f°C)\n", Config::PIN_FAN, fanOnTemp);
    Serial.printf("Cut-off Pin: %d (ACTIVE when temp >= %.0f°C)\n", Config::PIN_CUTOFF, cutoffTemp);
}

void CoolingSystem::start() {
    systemActive = true;
    
    digitalWrite(Config::PIN_EWP, HIGH);
    ewpStatus = true;
    
    Serial.println("Cooling System STARTED - EWP ON");
}

void CoolingSystem::stop() {
    systemActive = false;
    
    digitalWrite(Config::PIN_EWP, LOW);
    digitalWrite(Config::PIN_FAN, LOW);
    digitalWrite(Config::PIN_CUTOFF, LOW);
    
    ewpStatus = false;
    fanStatus = false;
    cutoffStatus = false;
    
    Serial.println("Cooling System STOPPED - All components OFF");
}

void CoolingSystem::update(float temperature) {
    if (!systemActive) return;
    
    unsigned long currentTime = millis();
    if (currentTime - lastUpdate < Config::COOLING_UPDATE_INTERVAL) return;
    
    currentTemp = temperature;
    bool statusChanged = false;
    
    // FAN CONTROL with hysteresis
    if (currentTemp >= fanOnTemp && !fanStatus) {
        digitalWrite(Config::PIN_FAN, HIGH);
        fanStatus = true;
        statusChanged = true;
        Serial.printf("FAN ON - Temp: %.1f°C >= %.0f°C\n", currentTemp, fanOnTemp);
    } else if (currentTemp < (fanOnTemp - Config::TEMP_HYSTERESIS_FAN) && fanStatus) {
        digitalWrite(Config::PIN_FAN, LOW);
        fanStatus = false;
        statusChanged = true;
        Serial.printf("FAN OFF - Temp: %.1f°C < %.0f°C (hysteresis)\n", 
                      currentTemp, fanOnTemp - Config::TEMP_HYSTERESIS_FAN);
    }
    
    // CUT OFF RELAY with hysteresis
    if (currentTemp >= cutoffTemp && !cutoffStatus) {
        digitalWrite(Config::PIN_CUTOFF, HIGH);
        cutoffStatus = true;
        statusChanged = true;
        Serial.printf("*** EMERGENCY CUT OFF ACTIVATED - Temp: %.1f°C >= %.0f°C ***\n", 
                      currentTemp, cutoffTemp);
    } else if (currentTemp < (cutoffTemp - Config::TEMP_HYSTERESIS_CUTOFF) && cutoffStatus) {
        digitalWrite(Config::PIN_CUTOFF, LOW);
        cutoffStatus = false;
        statusChanged = true;
        Serial.printf("CUT OFF DEACTIVATED - Temp: %.1f°C < %.0f°C (hysteresis)\n", 
                      currentTemp, cutoffTemp - Config::TEMP_HYSTERESIS_CUTOFF);
    }
    
    if (statusChanged) {
        Serial.printf("Cooling Status - EWP:%s FAN:%s CUTOFF:%s Temp:%.1f°C\n",
                      ewpStatus ? "ON" : "OFF",
                      fanStatus ? "ON" : "OFF", 
                      cutoffStatus ? "ACTIVE" : "OFF",
                      currentTemp);
    }
    
    lastUpdate = currentTime;
}

void CoolingSystem::emergencyShutdown() {
    Serial.println("EMERGENCY SHUTDOWN INITIATED!");
    digitalWrite(Config::PIN_CUTOFF, HIGH);
   
    cutoffStatus = true;
}

void CoolingSystem::setFanOnTemp(float temp) {
    fanOnTemp = temp;
    Serial.printf("Fan ON temperature set to: %.0f°C\n", fanOnTemp);
}

void CoolingSystem::setCutoffTemp(float temp) {
    cutoffTemp = temp;
    Serial.printf("Cut-off temperature set to: %.0f°C\n", cutoffTemp);
}

String CoolingSystem::getStatusText() const {
    String status = "";
    if (systemActive) {
        status += "EWP:ON ";
        status += fanStatus ? "FAN:ON " : "FAN:OFF ";
        if (cutoffStatus) {
            status += "CUTOFF:ACTIVE";
        } else {
            status += "CUTOFF:OFF";
        }
    } else {
        status = "COOLING:OFF";
    }
    return status;
}

uint16_t CoolingSystem::getStatusColor() const {
    if (!systemActive) return ST77XX_GRAY;
    if (cutoffStatus) return ST77XX_RED;
    if (fanStatus) return ST77XX_YELLOW;
    return ST77XX_GREEN;
}
