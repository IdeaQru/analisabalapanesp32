#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include "Config.h"
// === ENUMS ===
enum class LapDetectionMode {
    GPS_RETURN_TO_START = 0,
    DISTANCE_BASED = 1,
    TIME_BASED = 2
};

enum class MenuState {
    MAIN = 0,
    LAP_CONFIG = 1,
    DISPLAY_CONFIG = 2,
    COOLING_CONFIG = 3,
    LAP_COUNT = 4,
    DISTANCE_SET = 5,
    TIME_SET = 6
};

enum class SystemStatus {
    IDLE,
    RECORDING,
    TRANSMITTING,
    MENU,
    ERROR,
    EMERGENCY
};

// === DATA STRUCTURES ===
struct TrainingData {
    float afr;
    float rpm;
    float temp;
    float tps;
    float map_value;
    int classification;
    
    TrainingData() : afr(0), rpm(0), temp(0), tps(0), map_value(0), classification(0) {}
    TrainingData(float a, float r, float t, float tp, float m, int c) 
        : afr(a), rpm(r), temp(t), tps(tp), map_value(m), classification(c) {}
};

struct TimeOption {
    unsigned long seconds;
    String displayText;
    
    TimeOption() : seconds(0), displayText("") {}
    TimeOption(unsigned long s, const String& text) : seconds(s), displayText(text) {}
};

// === TAMBAHAN INI - TIME OPTIONS ARRAY ===

struct LapConfiguration {
    LapDetectionMode mode;
    float targetDistance;
    unsigned long targetTime;
    float gpsThreshold;
    int totalLaps;
    int timeOptionIndex;
    
    LapConfiguration() 
        : mode(LapDetectionMode::DISTANCE_BASED), targetDistance(500.0f), 
          targetTime(120000), gpsThreshold(0.0005f), totalLaps(3), timeOptionIndex(7) {}
};

struct DisplayConfiguration {
    bool showGPS;
    bool showSensors;
    bool showClassification;
    bool showLapInfo;
    bool showDetailedStats;
    bool showProgressBar;
    bool showCoolingStatus;
    int refreshRate;
    
    DisplayConfiguration() 
        : showGPS(true), showSensors(true), showClassification(true), 
          showLapInfo(true), showDetailedStats(true), showProgressBar(true), 
          showCoolingStatus(true), refreshRate(Config::DEFAULT_REFRESH_RATE) {}
};

struct SensorData {
    int lapNumber;
    float afr;
    float rpm;
    float temp;
    float tps;
    float map_value;
    double lat;
    double lng;
    float speed;
    float incline;
    float stroke;
    unsigned long timestamp;
    
    SensorData() : lapNumber(0), afr(0), rpm(0), temp(0), tps(0), map_value(0),
                   lat(0), lng(0), speed(0), incline(0), stroke(0), timestamp(0) {}
    
    String toCSV() const {
        return String(lapNumber) + "," + String(afr, 1) + "," + String(rpm, 0) + "," +
               String(temp, 1) + "," + String(tps, 1) + "," + String(map_value, 1) + "," +
               String(lat, 6) + "," + String(lng, 6) + "," + String(speed, 1) + "," +
               String(incline, 1) + "," + String(stroke, 1) + "," + String(timestamp);
    }
};

struct LapStatistics {
    float maxSpeed;
    float avgSpeed;
    float maxRPM;
    float avgRPM;
    float maxTemp;
    float avgTemp;
    unsigned long bestLapTime;
    int totalDataPoints;
    float speedSum;
    float rpmSum;
    float tempSum;
    
    LapStatistics() : maxSpeed(0), avgSpeed(0), maxRPM(0), avgRPM(0), 
                      maxTemp(0), avgTemp(0), bestLapTime(0), totalDataPoints(0),
                      speedSum(0), rpmSum(0), tempSum(0) {}
    
    void reset() {
        maxSpeed = avgSpeed = maxRPM = avgRPM = maxTemp = avgTemp = 0;
        bestLapTime = totalDataPoints = 0;
        speedSum = rpmSum = tempSum = 0;
    }
    
    void update(const SensorData& data) {
        totalDataPoints++;
        
        if (data.speed > maxSpeed) maxSpeed = data.speed;
        if (data.rpm > maxRPM) maxRPM = data.rpm;
        if (data.temp > maxTemp) maxTemp = data.temp;
        
        speedSum += data.speed;
        rpmSum += data.rpm;
        tempSum += data.temp;
        
        avgSpeed = speedSum / totalDataPoints;
        avgRPM = rpmSum / totalDataPoints;
        avgTemp = tempSum / totalDataPoints;
    }
};

struct Distance {
    float distance;
    int classification;
    
    Distance() : distance(0), classification(0) {}
    Distance(float d, int c) : distance(d), classification(c) {}
};

#endif // DATA_STRUCTURES_H
