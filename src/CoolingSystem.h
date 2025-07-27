#ifndef COOLING_SYSTEM_H
#define COOLING_SYSTEM_H

#include "DataStructures.h"

class CoolingSystem {
private:
    bool ewpStatus;
    bool fanStatus;
    bool cutoffStatus;
    float fanOnTemp;
    float cutoffTemp;
    unsigned long lastUpdate;
    float currentTemp;
    bool systemActive;
    
public:
    CoolingSystem();
    ~CoolingSystem();
    
    void initialize();
    void start();
    void stop();
    void update(float temperature);
    void emergencyShutdown();
    
    // Getters
    bool isSystemActive() const { return systemActive; }
    bool isEWPOn() const { return ewpStatus; }
    bool isFanOn() const { return fanStatus; }
    bool isCutoffActive() const { return cutoffStatus; }
    float getCurrentTemp() const { return currentTemp; }
    float getFanOnTemp() const { return fanOnTemp; }
    float getCutoffTemp() const { return cutoffTemp; }
    
    // Setters
    void setFanOnTemp(float temp);
    void setCutoffTemp(float temp);
    
    // Status methods
    String getStatusText() const;
    uint16_t getStatusColor() const;
    
    // Static methods untuk akses global
    static CoolingSystem& getInstance() {
        static CoolingSystem instance;
        return instance;
    }
};

#endif // COOLING_SYSTEM_H
