// #ifndef SYSTEM_MONITOR_H
// #define SYSTEM_MONITOR_H

// #include "DataStructures.h"

// class SystemMonitor {
// private:
//     unsigned long lastHealthCheck;
//     unsigned long lastMemoryCheck;
//     unsigned long lastErrorLog;
    
//     // LED control
//     bool ledState;
//     unsigned long lastLEDBlink;
    
//     // Error tracking
//     int errorCount;
//     String lastError;
    
//     // Private methods
//     void checkMemoryUsage();
//     void checkGPSStatus();
//     void checkTemperatureStatus();
//     void checkSystemComponents();
//     void updateLEDStatus();
//     void logSystemStatus();

// public:
//     SystemMonitor();
//     ~SystemMonitor();
    
//     void initialize();
//     void update();
    
//     // Health monitoring
//     void performHealthCheck();
//     void logError(const String& error);
//     void logWarning(const String& warning);
//     void logInfo(const String& info);
    
//     // System status
//     bool isSystemHealthy();
//     float getMemoryUsagePercent();
//     int getErrorCount() const { return errorCount; }
//     String getLastError() const { return lastError; }
    
//     // Emergency handling
//     void handleEmergency(const String& reason);
//     void recoverFromError();
    
//     // Static methods untuk akses global
//     static SystemMonitor& getInstance() {
//         static SystemMonitor instance;
//         return instance;
//     }
// };

// #endif // SYSTEM_MONITOR_H
