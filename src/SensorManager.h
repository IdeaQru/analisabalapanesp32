#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include "DataStructures.h"

class SensorManager
{
private:
    TinyGPSPlus *gps;
    HardwareSerial *gpsSerial;
    DallasTemperature *tempSensor;
    OneWire *oneWire;
    // RPM sensor variables
    static SensorManager* instance;
    volatile unsigned long rpmPulseCount;
    unsigned long lastRPMCalculation;
    float currentRPM;

    SensorData currentData;
    unsigned long lastSensorUpdate;
    unsigned long lastGPSUpdate;
    unsigned long lastTime;
   static void IRAM_ATTR rpmInterruptHandler();
    float readRPMSensor();
    // Private sensor reading methods
    float readAFRSensor();
    float readMAPSensor();
    float readTPSSensor();
    float readInclineSensor();
    float readStrokeSensor();
    float estimateRPM();

public:
    SensorManager();
    ~SensorManager();
    void initialize();
    void update();
    void updateGPS();
    float readTemperatureSensor();

    // Getters
    const SensorData &getCurrentData() const { return currentData; }
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
    static SensorManager &getInstance()
    {
        static SensorManager instance;
        return instance;
    }
};

#endif // SENSOR_MANAGER_H
