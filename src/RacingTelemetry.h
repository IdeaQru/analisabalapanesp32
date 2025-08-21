#ifndef RACING_TELEMETRY_H
#define RACING_TELEMETRY_H

#include "DataStructures.h"
#include "KNNClassifier.h"
#include "CoolingSystem.h"
#include "SensorManager.h"
#include "DisplayManager.h"
#include "ButtonHandler.h"
#include "RecordingManager.h"
#include "SystemMonitor.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

/**
 * @brief Main Racing Telemetry System Class
 */
class RacingTelemetry {
private:
    // === SYSTEM COMPONENTS ===
    KNNClassifier* classifier;          // AI classification engine
    CoolingSystem* coolingSystem;       // Intelligent cooling management
    SensorManager* sensorManager;       // Sensor data collection
    DisplayManager* displayManager;     // Animated display controller
    ButtonHandler* buttonHandler;       // User input management
    RecordingManager* recordingManager; // Data recording and transmission
    
    // === SYSTEM STATE ===
    SystemStatus currentStatus;         // Current system operating state
    LapConfiguration lapConfig;         // Lap detection configuration
    DisplayConfiguration displayConfig; // Display settings configuration
    
    // === TIMING CONTROL ===
    unsigned long lastUpdate;           // Last main update timestamp
    unsigned long lastClassification;   // Last AI classification timestamp
    
    // === AI CLASSIFICATION STATE ===
    int currentClassification;          // Current AI classification result
    String classificationText;          // Human-readable classification
    
    // === API CONFIGURATION ===
    String apiEndpoint;
    String apiKey;
    String deviceId;
    const char* ssid;
    const char* password;
    
    // === PRIVATE METHODS ===
    void updateAIClassification();
    void handleEmergencyCondition(const String& reason);
    void handleSerialCommand(const String& command);
    String getStatusText() const;
    
    // === API METHODS ===
    bool connectToWiFi();
    bool sendDataToAPI(const String& jsonData);
    String prepareTelemetryJSON();
    void handleAPIResponse(int httpCode, const String& response);
    void sendCurrentDataToAPI();  // ← TAMBAHAN INI
    
    // === PRINT STATUS METHODS ===
    void printSystemStatus();
    void printCoolingStatus();
    void printMemoryStatus();
    void printGPSStatus();
    void printSensorStatus();
    void printAIStatus();
    void printWiFiStatus();        // ← TAMBAHAN INI
    void printHelpMenu();
    void performSystemReset();
    void toggleDebugMode();
    void testAPIConnection();      // ← TAMBAHAN INI

public:
    // === PUBLIC MEMBERS ===
    bool serialActive = false;
    
    // === CONSTRUCTOR & DESTRUCTOR ===
    RacingTelemetry();
    ~RacingTelemetry();
    
    // === SYSTEM LIFECYCLE ===
    void initialize();
    void update();
    void handleSerialInput();
    
    // === SYSTEM STATE GETTERS ===
    SystemStatus getStatus() const { return currentStatus; }
    int getCurrentClassification() const { return currentClassification; }
    String getClassificationText() const { return classificationText; }
    
    // === RECORDING CONTROL ===
    void startRecording();
    void stopRecording();
    void transmitData();
    
    // === MENU SYSTEM CONTROL ===
    void enterMenu();
    void exitMenu();
    
    // === CONFIGURATION ACCESS ===
    LapConfiguration& getLapConfig() { return lapConfig; }
    DisplayConfiguration& getDisplayConfig() { return displayConfig; }
    
    // === SINGLETON PATTERN ===
    static RacingTelemetry& getInstance() {
        static RacingTelemetry instance;
        return instance;
    }
    
    // === SYSTEM INFORMATION ===
    static String getVersion() { return "Racing Telemetry OOP v2.0"; }
    static String getBuildDate() { return __DATE__ " " __TIME__; }
    static String getSupportedFeatures() { 
        return "AI-Classification,Multi-Lap-Detection,Animated-Display,Intelligent-Cooling,Real-time-Monitoring,API-Integration"; 
    }
};

// === GLOBAL SYSTEM ACCESS MACROS ===
#define TELEMETRY_SYSTEM RacingTelemetry::getInstance()
#define SYSTEM_STATUS TELEMETRY_SYSTEM.getStatus()
#define AI_CLASSIFICATION TELEMETRY_SYSTEM.getCurrentClassification()

#endif // RACING_TELEMETRY_H
