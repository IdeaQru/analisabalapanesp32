// #include "SystemMonitor.h"
// #include "SensorManager.h"
// #include "CoolingSystem.h"
// #include "RacingTelemetry.h"

// SystemMonitor::SystemMonitor() 
//     : lastHealthCheck(0), lastMemoryCheck(0), lastErrorLog(0),
//       ledState(false), lastLEDBlink(0), errorCount(0), lastError("") {
// }

// SystemMonitor::~SystemMonitor() {
// }

// void SystemMonitor::initialize() {
//     pinMode(Config::LED_PIN, OUTPUT);
//     digitalWrite(Config::LED_PIN, LOW);
    
//     lastHealthCheck = 0;
//     lastMemoryCheck = 0;
//     lastErrorLog = 0;
//     errorCount = 0;
//     lastError = "";
    
//     Serial.println("=== System Monitor Initialized ===");
//     Serial.printf("LED Status Pin: %d\n", Config::LED_PIN);
//     Serial.println("Health monitoring active");
// }

// void SystemMonitor::update() {
//     unsigned long currentTime = millis();
    
//     // Perform health check
//     if (currentTime - lastHealthCheck >= Config::HEALTH_CHECK_INTERVAL) {
//         performHealthCheck();
//         lastHealthCheck = currentTime;
//     }
    
//     // Update LED status
//     updateLEDStatus();
// }

// void SystemMonitor::performHealthCheck() {
//     checkMemoryUsage();
//     checkGPSStatus();
//     checkTemperatureStatus();
//     checkSystemComponents();
//     logSystemStatus();
// }

// void SystemMonitor::checkMemoryUsage() {
//     size_t freeHeap = ESP.getFreeHeap();
//     size_t totalHeap = ESP.getHeapSize();
    
//     if (freeHeap < Config::MIN_FREE_HEAP) {
//         logWarning("Low memory detected: " + String(freeHeap) + " bytes free");
        
//         // Try to recover memory if not recording
//         RacingTelemetry& system = RacingTelemetry::getInstance();
//         if (system.getStatus() != SystemStatus::RECORDING) {
//             // Could implement memory cleanup here
//         }
//     }
    
//     // Log memory usage periodically
//     static unsigned long lastMemLog = 0;
//     if (millis() - lastMemLog >= 60000) {
//         float usagePercent = ((float)(totalHeap - freeHeap) / totalHeap) * 100;
//         Serial.printf("MEMORY_STATUS: Free=%d, Total=%d (%.1f%% used)\n",
//                       freeHeap, totalHeap, usagePercent);
//         lastMemLog = millis();
//     }
// }

// void SystemMonitor::checkGPSStatus() {
//     SensorManager& sensors = SensorManager::getInstance();
//     RacingTelemetry& system = RacingTelemetry::getInstance();
    
//     if (!sensors.isGPSValid() && system.getStatus() == SystemStatus::RECORDING) {
//         logWarning("GPS signal lost during recording!");
//     }
    
//     if (sensors.getSatelliteCount() < 4 && sensors.getSatelliteCount() > 0) {
//         logWarning("Low satellite count: " + String(sensors.getSatelliteCount()));
//     }
// }

// void SystemMonitor::checkTemperatureStatus() {
//     CoolingSystem& cooling = CoolingSystem::getInstance();
    
//     if (cooling.getCurrentTemp() > 110.0f && !cooling.isCutoffActive()) {
//         logWarning("High temperature detected: " + String(cooling.getCurrentTemp()) + "°C");
//     }
    
//     if (cooling.getCurrentTemp() > cooling.getCutoffTemp()) {
//         logError("Temperature exceeds cut-off threshold: " + 
//                 String(cooling.getCurrentTemp()) + "°C >= " + 
//                 String(cooling.getCutoffTemp()) + "°C");
//     }
    
//     // Check for sensor malfunction
//     if (cooling.getCurrentTemp() < -20.0f || cooling.getCurrentTemp() > 200.0f) {
//         logError("Temperature sensor reading out of range: " + 
//                 String(cooling.getCurrentTemp()) + "°C");
//     }
// }

// void SystemMonitor::checkSystemComponents() {
//     // Check if all major components are responsive
//     // This could include checking if objects are properly initialized
    
//     SensorManager& sensors = SensorManager::getInstance();
//     CoolingSystem& cooling = CoolingSystem::getInstance();
    
//     // Example checks
//     if (&sensors == nullptr) {
//         logError("Sensor Manager not initialized");
//     }
    
//     if (&cooling == nullptr) {
//         logError("Cooling System not initialized");
//     }
// }

// void SystemMonitor::updateLEDStatus() {
//     RacingTelemetry& system = RacingTelemetry::getInstance();
//     CoolingSystem& cooling = CoolingSystem::getInstance();
    
//     unsigned long currentTime = millis();
//     int blinkInterval = 1000; // Default slow blink
    
//     switch (system.getStatus()) {
//         case SystemStatus::RECORDING:
//             blinkInterval = 200; // Fast blink during recording
//             break;
//         case SystemStatus::TRANSMITTING:
//             digitalWrite(Config::LED_PIN, HIGH); // Solid ON during transmission
//             return;
//         case SystemStatus::EMERGENCY:
//             blinkInterval = 100; // Very fast blink during emergency
//             break;
//         default:
//             if (cooling.isCutoffActive()) {
//                 blinkInterval = 100; // Very fast blink for cooling emergency
//             } else if (cooling.isFanOn()) {
//                 blinkInterval = 500; // Medium blink when fan is running
//             }
//             break;
//     }
    
//     // Blink LED
//     if (currentTime - lastLEDBlink >= blinkInterval) {
//         ledState = !ledState;
//         digitalWrite(Config::LED_PIN, ledState ? HIGH : LOW);
//         lastLEDBlink = currentTime;
//     }
// }

// void SystemMonitor::logSystemStatus() {
//     RacingTelemetry& system = RacingTelemetry::getInstance();
//     SensorManager& sensors = SensorManager::getInstance();
//     CoolingSystem& cooling = CoolingSystem::getInstance();
    
//     if (system.getStatus() != SystemStatus::RECORDING && 
//         system.getStatus() != SystemStatus::TRANSMITTING) {
//         size_t freeHeap = ESP.getFreeHeap();
//         Serial.printf("HEALTH_CHECK:Heap=%d,GPS=%s,Temp=%.1f,Cool=%s,Errors=%d\n",
//                       freeHeap,
//                       sensors.isGPSValid() ? "OK" : "NO_FIX",
//                       cooling.getCurrentTemp(),
//                       cooling.isSystemActive() ? "ACTIVE" : "STANDBY",
//                       errorCount);
//     }
// }

// void SystemMonitor::logError(const String& error) {
//     errorCount++;
//     lastError = error;
//     Serial.printf("ERROR: %s\n", error.c_str());
    
//     // Log to file if possible
//     File errorLog = SPIFFS.open("/error.log", "a");
//     if (errorLog) {
//         errorLog.printf("[%lu] ERROR: %s\n", millis(), error.c_str());
//         errorLog.close();
//     }
// }

// void SystemMonitor::logWarning(const String& warning) {
//     Serial.printf("WARNING: %s\n", warning.c_str());
    
//     // Log to file if possible
//     File errorLog = SPIFFS.open("/error.log", "a");
//     if (errorLog) {
//         errorLog.printf("[%lu] WARNING: %s\n", millis(), warning.c_str());
//         errorLog.close();
//     }
// }

// void SystemMonitor::logInfo(const String& info) {
//     Serial.printf("INFO: %s\n", info.c_str());
// }

// bool SystemMonitor::isSystemHealthy() {
//     // Define system health criteria
//     size_t freeHeap = ESP.getFreeHeap();
//     CoolingSystem& cooling = CoolingSystem::getInstance();
    
//     return (freeHeap > Config::MIN_FREE_HEAP && 
//             !cooling.isCutoffActive() && 
//             errorCount < 10);
// }

// float SystemMonitor::getMemoryUsagePercent() {
//     size_t freeHeap = ESP.getFreeHeap();
//     size_t totalHeap = ESP.getHeapSize();
//     return ((float)(totalHeap - freeHeap) / totalHeap) * 100;
// }

// void SystemMonitor::handleEmergency(const String& reason) {
//     logError("EMERGENCY: " + reason);
    
//     RacingTelemetry& system = RacingTelemetry::getInstance();
    
//     // Stop recording if active
//     if (system.getStatus() == SystemStatus::RECORDING) {
//         system.stopRecording();
//     }
    
//     // Activate emergency cooling if temperature related
//     if (reason.indexOf("TEMPERATURE") >= 0 || reason.indexOf("OVERHEAT") >= 0) {
//         CoolingSystem& cooling = CoolingSystem::getInstance();
//         cooling.emergencyShutdown();
//     }
    
//     // Set system to emergency state
//     // This would need to be implemented in RacingTelemetry class
// }

// void SystemMonitor::recoverFromError() {
//     Serial.println("Attempting system recovery...");
    
//     // Reset error count
//     errorCount = 0;
//     lastError = "";
    
//     // Try to reinitialize critical components
//     SensorManager& sensors = SensorManager::getInstance();
//     sensors.initialize();
    
//     Serial.println("System recovery completed");
// }
