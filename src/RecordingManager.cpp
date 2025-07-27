#include "RecordingManager.h"
#include "SensorManager.h"
#include "CoolingSystem.h"
#include "SystemMonitor.h"

RecordingManager::RecordingManager() 
    : isRecording(false), isTransmitting(false), currentLap(1), 
      currentLapDistance(0.0f), lastLat(0.0), lastLng(0.0), hasLastPosition(false),
      firstLapLat(0.0), firstLapLng(0.0), firstLapSet(false), lapStartTime(0),
      lapConfig(nullptr), dataFileName("/telemetry_data.txt") {
    
    // Initialize statistics
    currentLapStats.reset();
    overallStats.reset();
}

RecordingManager::~RecordingManager() {
    if (isRecording) {
        stopRecording();
    }
}

void RecordingManager::initialize() {
    Serial.println("=== Recording Manager Initializing ===");
    
    // Initialize SPIFFS
    if (!SPIFFS.begin(true)) {
        Serial.println("ERROR: SPIFFS initialization failed!");
        return;
    }
    
    // Reset all states
    isRecording = false;
    isTransmitting = false;
    currentLap = 1;
    currentLapDistance = 0.0f;
    hasLastPosition = false;
    firstLapSet = false;
    lapStartTime = 0;
    
    // Reset statistics
    currentLapStats.reset();
    overallStats.reset();
    
    // SPIFFS information
    size_t totalBytes = SPIFFS.totalBytes();
    size_t usedBytes = SPIFFS.usedBytes();
    size_t freeBytes = totalBytes - usedBytes;
    
    Serial.println("=== Recording Manager Initialized ===");
    Serial.printf("Data file: %s\n", dataFileName.c_str());
    Serial.printf("SPIFFS Total: %d bytes (%.1f KB)\n", totalBytes, totalBytes / 1024.0f);
    Serial.printf("SPIFFS Used: %d bytes (%.1f KB)\n", usedBytes, usedBytes / 1024.0f);
    Serial.printf("SPIFFS Free: %d bytes (%.1f KB)\n", freeBytes, freeBytes / 1024.0f);
    
    // Check if old data exists
    if (SPIFFS.exists(dataFileName)) {
        File file = SPIFFS.open(dataFileName, "r");
        if (file) {
            Serial.printf("Previous data file found: %d bytes\n", file.size());
            file.close();
        }
    }
}

void RecordingManager::startRecording() {
    if (isRecording) {
        Serial.println("WARNING: Already recording!");
        return;
    }
    
    if (isTransmitting) {
        Serial.println("ERROR: Cannot start recording while transmitting!");
        return;
    }
    
    Serial.println("=== STARTING RECORDING ===");
    
    // **HAPUS SEMUA DATA SPIFFS LAMA**
    Serial.println("Clearing all SPIFFS data...");
    if (!SPIFFS.format()) {
        Serial.println("ERROR: Failed to format SPIFFS!");
        return;
    }
    Serial.println("SPIFFS formatted successfully - all old data cleared");
    
    // Start cooling system if not active
    CoolingSystem& cooling = CoolingSystem::getInstance();
    if (!cooling.isSystemActive()) {
        cooling.start();
        Serial.println("Auto-starting cooling system for recording");
    }
    
    // Reset recording state
    isRecording = true;
    currentLap = 1;
    currentLapDistance = 0.0f;
    lapStartTime = millis();
    
    // Reset statistics
    currentLapStats.reset();
    overallStats.reset();
    
    // Initialize lap detection
    initializeLapDetection();
    
    // Create new data file
    createDataFile();
    
    Serial.printf("RECORDING STARTED: %d laps planned, Cooling: %s\n", 
                  lapConfig ? lapConfig->totalLaps : 3,
                  cooling.isSystemActive() ? "ACTIVE" : "INACTIVE");
    
    if (lapConfig) {
        Serial.printf("Lap Mode: %d, Target Distance: %.0fm, Target Time: %ds\n",
                      static_cast<int>(lapConfig->mode), 
                      lapConfig->targetDistance, 
                      lapConfig->targetTime);
    }
}

void RecordingManager::stopRecording() {
    if (!isRecording) {
        Serial.println("WARNING: Not currently recording!");
        return;
    }
    
    Serial.println("=== STOPPING RECORDING ===");
    
    // Complete current lap if it has meaningful data
    unsigned long currentLapTime = millis() - lapStartTime;
    if (currentLapTime > 10000) { // At least 10 seconds of data
        completeLap();
    }
    
    isRecording = false;
    
    // Close data file with summary
    closeDataFile();
    
    Serial.printf("RECORDING COMPLETED: %d laps, Max Temp: %.1f°C\n", 
                  currentLap - 1, overallStats.maxTemp);
    Serial.printf("Best Lap Time: %lu ms, Max Speed: %.1f km/h\n",
                  overallStats.bestLapTime, overallStats.maxSpeed);
}

void RecordingManager::update() {
    if (!isRecording) return;
    
    // Update lap progress
    updateLapProgress();
    
    // Save current sensor data periodically
    static unsigned long lastDataSave = 0;
    if (millis() - lastDataSave > 1000) { // Save every second
        saveCurrentSensorData();
        lastDataSave = millis();
    }
    
    // Check if all laps completed
    if (lapConfig && currentLap > lapConfig->totalLaps) {
        Serial.println("All laps completed - stopping recording");
        stopRecording();
        return;
    }
    
    // Emergency stop for overheating
    CoolingSystem& cooling = CoolingSystem::getInstance();
    if (cooling.getCurrentTemp() >= cooling.getCutoffTemp()) {
        Serial.printf("EMERGENCY STOP: Critical overheating %.1f°C >= %.1f°C\n",
                      cooling.getCurrentTemp(), cooling.getCutoffTemp());
        stopRecording();
        return;
    }
    
    // Check for excessive lap time (safety feature)
    unsigned long currentLapTime = millis() - lapStartTime;
    if (currentLapTime > 1800000) { // 30 minutes max per lap
        Serial.println("WARNING: Lap time exceeded 30 minutes - completing lap");
        completeLap();
    }
}

void RecordingManager::updateLapProgress() {
    if (!isRecording || !lapConfig) return;
    
    SensorManager& sensors = SensorManager::getInstance();
    if (!sensors.isGPSValid()) return;
    
    double currentLat = sensors.getLatitude();
    double currentLng = sensors.getLongitude();
    
    switch (lapConfig->mode) {
        case LapDetectionMode::DISTANCE_BASED: {
            if (hasLastPosition) {
                double segmentDistance = calculateDistance(lastLat, lastLng, currentLat, currentLng);
                currentLapDistance += segmentDistance;
                
                if (currentLapDistance >= lapConfig->targetDistance) {
                    Serial.printf("Distance lap completed: %.1fm >= %.1fm\n", 
                                  currentLapDistance, lapConfig->targetDistance);
                    completeLap();
                    currentLapDistance = 0.0f;
                }
            }
            lastLat = currentLat;
            lastLng = currentLng;
            hasLastPosition = true;
            break;
        }
        
        case LapDetectionMode::TIME_BASED: {
            unsigned long elapsed = millis() - lapStartTime;
            if (elapsed >= (lapConfig->targetTime * 1000)) {
                Serial.printf("Time lap completed: %lu ms >= %lu ms\n", 
                              elapsed, lapConfig->targetTime * 1000);
                completeLap();
            }
            break;
        }
        
        case LapDetectionMode::GPS_RETURN_TO_START: {
            if (!firstLapSet && currentLap == 1) {
                firstLapLat = currentLat;
                firstLapLng = currentLng;
                firstLapSet = true;
                Serial.printf("GPS start position set: %.6f, %.6f\n", firstLapLat, firstLapLng);
                return;
            }
            
            if (firstLapSet) {
                double distance = calculateDistance(firstLapLat, firstLapLng, currentLat, currentLng);
                double thresholdMeters = lapConfig->gpsThreshold * 111000; // Convert degrees to meters
                
                if (distance < thresholdMeters && (millis() - lapStartTime) > 30000) {
                    Serial.printf("GPS return lap completed: %.1fm < %.1fm threshold\n", 
                                  distance, thresholdMeters);
                    completeLap();
                }
            }
            break;
        }
        
        default:
            Serial.println("WARNING: Unknown lap detection mode!");
            break;
    }
}

void RecordingManager::completeLap() {
    unsigned long lapTime = millis() - lapStartTime;
    
    // Update lap statistics
    currentLapStats.bestLapTime = lapTime;
    
    // Update overall statistics
    if (overallStats.bestLapTime == 0 || lapTime < overallStats.bestLapTime) {
        overallStats.bestLapTime = lapTime;
    }
    
    // Log lap completion
    Serial.printf("=== LAP %d COMPLETED ===\n", currentLap);
    Serial.printf("Lap Time: %lu ms (%.2f seconds)\n", lapTime, lapTime / 1000.0f);
    Serial.printf("Max Speed: %.1f km/h\n", currentLapStats.maxSpeed);
    Serial.printf("Max RPM: %.0f\n", currentLapStats.maxRPM);
    Serial.printf("Max Temp: %.1f°C\n", currentLapStats.maxTemp);
    
    if (lapConfig) {
        Serial.printf("Total Laps: %d/%d\n", currentLap, lapConfig->totalLaps);
    }
    
    // Save lap summary to file
    appendLapSummaryToFile(currentLap, lapTime);
    
    // Move to next lap
    currentLap++;
    lapStartTime = millis();
    
    // Reset current lap statistics for next lap
    currentLapStats.reset();
    
    // Reset lap-specific data
    if (lapConfig && lapConfig->mode == LapDetectionMode::DISTANCE_BASED) {
        currentLapDistance = 0.0f;
    }
}

void RecordingManager::saveCurrentSensorData() {
    if (!isRecording) return;
    
    SensorManager& sensors = SensorManager::getInstance();
    const SensorData& data = sensors.getCurrentData();
    
    // Update current lap statistics
    currentLapStats.update(data);
    
    // Update overall statistics
    overallStats.update(data);
    
    // Save to file
    appendDataToFile(data);
    
    // Debug output occasionally
    static unsigned long lastDebug = 0;
    if (millis() - lastDebug > 10000) { // Every 10 seconds
        Serial.printf("Recording - Lap: %d, Time: %lu s, Temp: %.1f°C, Speed: %.1f km/h\n",
                      currentLap, (millis() - lapStartTime) / 1000, 
                      data.temp, data.speed);
        lastDebug = millis();
    }
}

void RecordingManager::createDataFile() {
    // Remove existing file
    if (SPIFFS.exists(dataFileName)) {
        SPIFFS.remove(dataFileName);
        Serial.println("Removed existing data file");
    }
    
    // Create new file
    File file = SPIFFS.open(dataFileName, "w");
    if (!file) {
        Serial.println("ERROR: Failed to create data file");
        return;
    }
    
    // Write CSV header
    file.println("timestamp,lapNumber,afr,rpm,temperature,tps,map,latitude,longitude,speed,incline,stroke");
    
    // Write recording metadata
    file.printf("# Recording started at: %lu\n", millis());
    file.printf("# System timestamp: %s\n", String(millis()).c_str());
    
    if (lapConfig) {
        file.printf("# Lap Configuration:\n");
        file.printf("#   Mode: %d (%s)\n", static_cast<int>(lapConfig->mode),
                    (lapConfig->mode == LapDetectionMode::DISTANCE_BASED) ? "Distance" :
                    (lapConfig->mode == LapDetectionMode::TIME_BASED) ? "Time" : "GPS Return");
        file.printf("#   Total Laps: %d\n", lapConfig->totalLaps);
        file.printf("#   Target Distance: %.1f meters\n", lapConfig->targetDistance);
        file.printf("#   Target Time: %d seconds\n", lapConfig->targetTime);
        file.printf("#   GPS Threshold: %.6f degrees\n", lapConfig->gpsThreshold);
    }
    
    // Write cooling system info
    CoolingSystem& cooling = CoolingSystem::getInstance();
    file.printf("# Cooling System:\n");
    file.printf("#   Active: %s\n", cooling.isSystemActive() ? "YES" : "NO");
    file.printf("#   Fan Temperature: %.0f°C\n", cooling.getFanOnTemp());
    file.printf("#   Cut-off Temperature: %.0f°C\n", cooling.getCutoffTemp());
    
    file.close();
    Serial.println("Data file created successfully");
}

void RecordingManager::appendDataToFile(const SensorData& data) {
    File file = SPIFFS.open(dataFileName, "a");
    if (!file) {
        Serial.println("ERROR: Failed to open file for appending data");
        return;
    }
    
    // **PERBAIKAN 1: Format sesuai dengan Qt application requirements**
    // lapNumber,afr,rpm,temp,tps,map,lat,lng,speed,incline,stroke,timestamp
    file.printf("%d,%.1f,%.0f,%.0f,%.0f,%.0f,%.4f,%.4f,%.0f,%.0f,%.0f,%lu\n",
                currentLap,         // lapNumber (integer)
                data.afr,          // afr (1 decimal)
                data.rpm,          // rpm (no decimal)
                data.temp,         // temperature (no decimal) 
                data.tps,          // tps (no decimal)
                data.map_value,    // map (no decimal)
                data.lat,          // latitude (4 decimals for GPS precision)
                data.lng,          // longitude (4 decimals for GPS precision)
                data.speed,        // speed (no decimal)
                data.incline,      // incline (no decimal)
                data.stroke,       // stroke (no decimal, FIXED trailing comma)
                millis()           // timestamp (moved to end, no trailing comma)
    );
    
    // **PERBAIKAN 2: Error handling dan validation**
 
    
    file.close();
    
   
    
    // **PERBAIKAN 4: Periodic status update**
    static unsigned long lastStatusUpdate = 0;
    unsigned long currentTime = millis();
    
    
}

void RecordingManager::appendLapSummaryToFile(int lapNumber, unsigned long lapTime) {
    File file = SPIFFS.open(dataFileName, "a");
    if (!file) {
        Serial.println("ERROR: Failed to open file for lap summary");
        return;
    }
    
    file.printf("# LAP %d SUMMARY:\n", lapNumber);
    file.printf("#   Completed at: %lu ms\n", millis());
    file.printf("#   Lap Time: %lu ms (%.2f seconds)\n", lapTime, lapTime / 1000.0f);
    file.printf("#   Max Speed: %.1f km/h\n", currentLapStats.maxSpeed);
    file.printf("#   Max RPM: %.0f\n", currentLapStats.maxRPM);
    file.printf("#   Max Temperature: %.1f°C\n", currentLapStats.maxTemp);
    file.printf("#   Distance Traveled: %.1f meters\n", currentLapDistance);
    file.println("#");
    
    file.close();
}

void RecordingManager::closeDataFile() {
    File file = SPIFFS.open(dataFileName, "a");
    if (!file) {
        Serial.println("ERROR: Failed to open file for closing summary");
        return;
    }
    
    file.printf("# RECORDING COMPLETED AT: %lu ms\n", millis());
    file.println("# OVERALL STATISTICS:");
    file.printf("#   Total Laps: %d\n", currentLap - 1);
    file.printf("#   Best Lap Time: %lu ms (%.2f seconds)\n", 
                overallStats.bestLapTime, overallStats.bestLapTime / 1000.0f);
    file.printf("#   Max Speed: %.1f km/h\n", overallStats.maxSpeed);
    file.printf("#   Max RPM: %.0f\n", overallStats.maxRPM);
    file.printf("#   Max Temperature: %.1f°C\n", overallStats.maxTemp);
    file.printf("#   File Size: %d bytes\n", file.size());
    
    file.close();
    
    Serial.printf("Data file closed - Final size: %d bytes\n", getDataFileSize());
}

double RecordingManager::calculateDistance(double lat1, double lng1, double lat2, double lng2) {
    // Haversine formula for calculating distance between two GPS points
    const double EARTH_RADIUS = 6371000.0; // Earth radius in meters
    
    double dLat = (lat2 - lat1) * PI / 180.0;
    double dLng = (lng2 - lng1) * PI / 180.0;
    
    double a = sin(dLat/2) * sin(dLat/2) + 
               cos(lat1 * PI / 180.0) * cos(lat2 * PI / 180.0) * 
               sin(dLng/2) * sin(dLng/2);
    
    double c = 2 * atan2(sqrt(a), sqrt(1-a));
    
    return EARTH_RADIUS * c; // Distance in meters
}

void RecordingManager::transmitAllData() {
    if (isRecording) {
        Serial.println("ERROR:STILL_RECORDING");  // Format SAMA dengan Program 1
        return;
    }
    
    if (isTransmitting) {
        Serial.println("ERROR:ALREADY_TRANSMITTING");
        return;
    }
    
    if (!SPIFFS.exists(dataFileName)) {
        Serial.println("ERROR:NO_DATA_FILE");  // Format SAMA dengan Program 1
        return;
    }
    
    File file = SPIFFS.open(dataFileName, "r");
    if (!file) {
        Serial.println("ERROR:NO_DATA_FILE");
        return;
    }
    
    isTransmitting = true;
    
    // FORMAT IDENTIK DENGAN PROGRAM 1
    Serial.println("TRANSMISSION_START");  // TANPA "==="
    
    // Send file info SAMA dengan Program 1
    int fileSize = file.size();
    Serial.printf("FILE_SIZE:%d\n", fileSize);  // Format SAMA
    
    int lineCount = 0;
    int bytesRead = 0;
    int lastProgress = 0;
    
    while (file.available()) {
        String line = file.readStringUntil('\n');
        bytesRead += line.length() + 1;
        line.trim();
        
        // HANYA KIRIM DATA, SKIP SEMUA METADATA DAN COMMENT
        if (line.length() > 0 && !line.startsWith("#")) {
            Serial.println(line);  // Data mentah tanpa format tambahan
            lineCount++;
            
            // Progress report SAMA dengan Program 1
            int progress = (bytesRead * 100) / fileSize;
            if (progress >= lastProgress + 10) {
                Serial.printf("PROGRESS:%d%%\n", progress);
                lastProgress = progress;
                Serial.flush();
            }
            
            // Yield SAMA dengan Program 1
            if (lineCount % 150 == 0) {
                yield();
            }
        }
    }
    
    file.close();
    
    // FORMAT IDENTIK DENGAN PROGRAM 1
    Serial.println("TRANSMISSION_END");  // TANPA "==="
    Serial.printf("TOTAL_LINES:%d\n", lineCount);  // Format SAMA
    
    isTransmitting = false;
}

void RecordingManager::handleSerialCommand(const String& command) {
    String cmd = command;
    cmd.trim();
    cmd.toUpperCase();
    
    Serial.printf("Processing command: '%s'\n", cmd.c_str());
    
    if (cmd == "START" || cmd == "1") {
        startRecording();
    }
    else if (cmd == "STOP") {
        if (isRecording) {
            stopRecording();
        } else {
            Serial.println("Not currently recording");
        }
    }
    else if (cmd == "TRANSMIT" || cmd == "2") {
        transmitAllData();
    }
    else if (cmd == "PAUSE") {
        pauseRecording();
    }
    else if (cmd == "RESUME") {
        resumeRecording();
    }
    else if (cmd == "STATUS") {
        Serial.printf("=== RECORDING MANAGER STATUS ===\n");
        Serial.printf("Recording: %s\n", isRecording ? "ACTIVE" : "INACTIVE");
        Serial.printf("Transmitting: %s\n", isTransmitting ? "ACTIVE" : "INACTIVE");
        Serial.printf("Current Lap: %d\n", currentLap);
        if (lapConfig) {
            Serial.printf("Total Laps: %d\n", lapConfig->totalLaps);
            Serial.printf("Lap Progress: %.1f%%\n", getLapProgress());
        }
        Serial.printf("Recording Time: %lu seconds\n", getRecordingTime() / 1000);
        Serial.printf("Data File: %s (%d bytes)\n", dataFileName.c_str(), getDataFileSize());
        Serial.printf("SPIFFS Used: %d / %d bytes\n", SPIFFS.usedBytes(), SPIFFS.totalBytes());
    }
    else if (cmd == "INFO") {
        Serial.printf("=== RECORDING STATISTICS ===\n");
        Serial.printf("Current Lap Stats:\n");
        Serial.printf("  Max Speed: %.1f km/h\n", currentLapStats.maxSpeed);
        Serial.printf("  Max RPM: %.0f\n", currentLapStats.maxRPM);
        Serial.printf("  Max Temp: %.1f°C\n", currentLapStats.maxTemp);
        Serial.printf("Overall Stats:\n");
        Serial.printf("  Best Lap: %lu ms\n", overallStats.bestLapTime);
        Serial.printf("  Max Speed: %.1f km/h\n", overallStats.maxSpeed);
        Serial.printf("  Max RPM: %.0f\n", overallStats.maxRPM);
        Serial.printf("  Max Temp: %.1f°C\n", overallStats.maxTemp);
    }
    else if (cmd == "DELETE") {
        if (isRecording || isTransmitting) {
            Serial.println("ERROR: Cannot delete data while recording or transmitting!");
        } else {
            if (SPIFFS.exists(dataFileName)) {
                SPIFFS.remove(dataFileName);
                Serial.println("Data file deleted successfully");
            } else {
                Serial.println("No data file to delete");
            }
        }
    }
    else {
        Serial.printf("Unknown command: '%s'\n", cmd.c_str());
        Serial.println("Available commands: START, STOP, TRANSMIT, PAUSE, RESUME, STATUS, INFO, DELETE");
    }
}

void RecordingManager::pauseRecording() {
    if (!isRecording) {
        Serial.println("Not currently recording - cannot pause");
        return;
    }
    
    isRecording = false;
    Serial.println("Recording PAUSED");
}

void RecordingManager::resumeRecording() {
    if (isRecording) {
        Serial.println("Already recording - cannot resume");
        return;
    }
    
    isRecording = true;
    Serial.println("Recording RESUMED");
}

void RecordingManager::initializeLapDetection() {
    currentLapDistance = 0.0f;
    hasLastPosition = false;
    firstLapSet = false;
    currentLapStats.reset();
    
    Serial.println("Lap detection initialized");
    
    if (lapConfig) {
        switch (lapConfig->mode) {
            case LapDetectionMode::DISTANCE_BASED:
                Serial.printf("Distance-based lap detection: %.0f meters per lap\n", lapConfig->targetDistance);
                break;
            case LapDetectionMode::TIME_BASED:
                Serial.printf("Time-based lap detection: %d seconds per lap\n", lapConfig->targetTime);
                break;
            case LapDetectionMode::GPS_RETURN_TO_START:
                Serial.printf("GPS return-to-start detection: %.6f degree threshold\n", lapConfig->gpsThreshold);
                // GPS start position will be set on first GPS fix
                break;
        }
    } else {
        Serial.println("WARNING: No lap configuration set - using defaults");
    }
}
