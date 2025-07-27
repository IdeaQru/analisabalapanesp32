#ifndef RECORDING_MANAGER_H
#define RECORDING_MANAGER_H

#include "DataStructures.h"
#include "SPIFFS.h"

class RecordingManager {
private:
    bool isRecording;
    bool isTransmitting;
    int currentLap;
    float currentLapDistance;
    double lastLat, lastLng;
    bool hasLastPosition;
    double firstLapLat, firstLapLng;
    bool firstLapSet;
    unsigned long lapStartTime;
    
    LapConfiguration* lapConfig;
    LapStatistics currentLapStats;
    LapStatistics overallStats;
    
    String dataFileName;
    String serialCmd;
    
    // Private methods
    void initializeLapDetection();
    void createDataFile();
    void appendDataToFile(const SensorData& data);
    void appendLapSummaryToFile(int lapNumber, unsigned long lapTime);
    void closeDataFile();
    double calculateDistance(double lat1, double lng1, double lat2, double lng2);

public:
    RecordingManager();
    ~RecordingManager();
    
    void initialize();
    void update();
    void updateLapProgress();
    void completeLap();
    
    // Recording control
    void startRecording();
    void stopRecording();
    void pauseRecording();
    void resumeRecording();
    
    // Data management
    void saveCurrentSensorData();
    void transmitAllData();
    void handleSerialCommand(const String& command);
    
    // Getters - sesuai dengan yang diperlukan DisplayManager
    bool getIsRecording() const { return isRecording; }
    bool getIsTransmitting() const { return isTransmitting; }
    int getCurrentLap() const { return currentLap; }
    float getCurrentLapDistance() const { return currentLapDistance; }
    const LapStatistics& getCurrentLapStats() const { return currentLapStats; }
    const LapStatistics& getOverallStats() const { return overallStats; }
    
    // Additional getters untuk DisplayManager integration
    int getTotalLaps() const { return lapConfig ? lapConfig->totalLaps : 3; }
    
    float getLapProgress() const {
        if (!lapConfig) return 0.0f;
        
        switch (lapConfig->mode) {
            case LapDetectionMode::DISTANCE_BASED:
                if (lapConfig->targetDistance > 0) {
                    return (currentLapDistance / lapConfig->targetDistance) * 100.0f;
                }
                return 0.0f;
            case LapDetectionMode::TIME_BASED:
                if (lapConfig->targetTime > 0) {
                    unsigned long elapsed = millis() - lapStartTime;
                    return (float(elapsed) / float(lapConfig->targetTime * 1000)) * 100.0f;
                }
                return 0.0f;
            default:
                return 0.0f;
        }
    }
    
    unsigned long getRecordingTime() const {
        return isRecording ? (millis() - lapStartTime) : 0;
    }
    
    unsigned long getCurrentLapTime() const {
        return isRecording ? (millis() - lapStartTime) : 0;
    }
    
    bool isCurrentlyRecording() const { return isRecording; }
    bool isCurrentlyTransmitting() const { return isTransmitting; }
    
    // Configuration
    void setLapConfiguration(LapConfiguration* config) { lapConfig = config; }
    LapConfiguration* getLapConfiguration() const { return lapConfig; }
    
    // File system info
    String getDataFileName() const { return dataFileName; }
    bool hasRecordedData() const { return SPIFFS.exists(dataFileName); }
    size_t getDataFileSize() const {
        if (SPIFFS.exists(dataFileName)) {
            File file = SPIFFS.open(dataFileName, "r");
            if (file) {
                size_t size = file.size();
                file.close();
                return size;
            }
        }
        return 0;
    }
    
    // Static methods untuk akses global
    static RecordingManager& getInstance() {
        static RecordingManager instance;
        return instance;
    }
};

#endif // RECORDING_MANAGER_H
