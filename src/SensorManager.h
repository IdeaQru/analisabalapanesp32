#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include "DataStructures.h"

class SensorManager {
private:
    TinyGPSPlus* gps;
    HardwareSerial* gpsSerial;
    DallasTemperature* tempSensor;
    OneWire* oneWire;
    
    SensorData currentData;
    unsigned long lastSensorUpdate;
    unsigned long lastGPSUpdate;
    
    // Private sensor reading methods
    float readAFRSensor();
    float readMAPSensor();
    float readTPSSensor();
    float readInclineSensor();
    float readStrokeSensor();
    float readTemperatureSensor();
    float estimateRPM();
    
public:
    SensorManager();
    ~SensorManager();
    
    void initialize();
    void update();
    void updateGPS();
    
    // Getters
    const SensorData& getCurrentData() const { return currentData; }
    bool isGPSValid() const;
    double getLatitude() const;
    double getLongitude() const;
    float getSpeed() const;
    int getSatelliteCount() const;
    float getCurrentTemperature() const { return currentData.temp; }
    
    // Utility methods
    void logSensorData();
    void setLapNumber(int lapNumber) { currentData.lapNumber = lapNumber; }
    
    // Static methods untuk akses global
    static SensorManager& getInstance() {
        static SensorManager instance;
        return instance;
    }
};

#endif // SENSOR_MANAGER_H
