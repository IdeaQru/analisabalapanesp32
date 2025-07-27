#include "RacingTelemetry.h"

RacingTelemetry::RacingTelemetry()
    : classifier(nullptr), coolingSystem(nullptr), sensorManager(nullptr),
      displayManager(nullptr), buttonHandler(nullptr), recordingManager(nullptr),
    //   systemMonitor(nullptr), currentStatus(SystemStatus::IDLE),
      lastUpdate(0), lastClassification(0), currentClassification(0),
      classificationText("Normal")
{
}

RacingTelemetry::~RacingTelemetry()
{
    // Objects are singletons, no need to delete
    Serial.println("Racing Telemetry System shutdown");
}

void RacingTelemetry::initialize()
{
    Serial.println("=== Racing Telemetry System Initializing ===");

    // Initialize random seed for animations
    randomSeed(analogRead(0));

    // Get singleton instances
    classifier = &KNNClassifier::getInstance();
    coolingSystem = &CoolingSystem::getInstance();
    sensorManager = &SensorManager::getInstance();
    displayManager = &DisplayManager::getInstance();
    buttonHandler = &ButtonHandler::getInstance();
    recordingManager = &RecordingManager::getInstance();
    // systemMonitor = &SystemMonitor::getInstance();

    // Verify all instances are valid
    if (!classifier || !coolingSystem || !sensorManager || !displayManager ||
        !buttonHandler || !recordingManager)
    {
        Serial.println("ERROR: Failed to get singleton instances!");
        return;
    }

    // Initialize all components in proper order
    Serial.println("Initializing components...");

    try
    {
        classifier->initialize();
        Serial.println("✓ KNN Classifier initialized");

        coolingSystem->initialize();
        Serial.println("✓ Cooling System initialized");

        sensorManager->initialize();
        Serial.println("✓ Sensor Manager initialized");

        displayManager->initialize();
        Serial.println("✓ Display Manager initialized");

        buttonHandler->initialize();
        Serial.println("✓ Button Handler initialized");

        recordingManager->initialize();
        Serial.println("✓ Recording Manager initialized");

        // systemMonitor->initialize();
        Serial.println("✓ System Monitor initialized");
    }
    catch (...)
    {
        Serial.println("ERROR: Exception during component initialization!");
        // systemMonitor->handleEmergency("Initialization failed");
        return;
    }

    // Set configurations
    displayManager->setConfiguration(&displayConfig);
    recordingManager->setLapConfiguration(&lapConfig);

    // Show animated startup screen
    displayManager->showStartupScreen();

    // Initialize system state
    currentStatus = SystemStatus::IDLE;
    lastUpdate = millis();
    lastClassification = millis();
    currentClassification = 0;
    classificationText = "Normal";

    // Perform initial system health check
    // systemMonitor->performHealthCheck();

    Serial.println("=== Racing Telemetry System Ready ===");
    Serial.printf("Training Data: %d samples, K=%d\n", Config::TRAIN_DATA_SIZE, Config::K_VALUE);
    Serial.printf("System Status: %d (0=IDLE)\n", static_cast<int>(currentStatus));
    Serial.println("All OOP components initialized successfully!");
    Serial.println("System ready for operation!");

    // Log system configuration
    Serial.println("=== System Configuration ===");
    Serial.printf("Lap Mode: %d, Target Laps: %d\n",
                  static_cast<int>(lapConfig.mode), lapConfig.totalLaps);
    Serial.printf("Display Refresh Rate: %d ms\n", displayConfig.refreshRate);
    Serial.printf("Cooling: Fan=%.0f°C, Cutoff=%.0f°C\n",
                  coolingSystem->getFanOnTemp(), coolingSystem->getCutoffTemp());
}

void RacingTelemetry::update()
{
    unsigned long currentTime = millis();

    // **OPTIMASI 1: Tingkatkan frequency untuk button responsiveness**
    if (currentTime - lastUpdate < 5) // Turun dari 10ms ke 5ms
        return; 

    try
    {
        // **PRIORITAS TINGGI: Update input handling PERTAMA dan SELALU**
        buttonHandler->update(); // Ini harus SELALU dipanggil

        // **OPTIMASI 2: Update sensors dengan interval yang lebih efisien**
        static unsigned long lastSensorUpdate = 0;
        if (currentTime - lastSensorUpdate >= 20) { // Sensor update setiap 20ms
            sensorManager->update();
            sensorManager->updateGPS();
            lastSensorUpdate = currentTime;
        }

        // **OPTIMASI 3: Cooling system dengan interval yang wajar**
        static unsigned long lastCoolingUpdate = 0;
        if (currentTime - lastCoolingUpdate >= 20) { // Cooling update setiap 100ms
            float currentTemp = sensorManager->getCurrentTemperature();
            coolingSystem->update(currentTemp);
            
            // Check for emergency conditions
            if (coolingSystem->isCutoffActive() && currentTemp >= coolingSystem->getCutoffTemp())
            {
                handleEmergencyCondition("Critical overheating detected");
            }
            lastCoolingUpdate = currentTime;
        }

        // **PRIORITAS TINGGI: Recording update (jika recording)**
        if (currentStatus == SystemStatus::RECORDING)
        {
            recordingManager->update();
            recordingManager->saveCurrentSensorData();
            sensorManager->setLapNumber(recordingManager->getCurrentLap());

            // Check if recording should auto-stop
            if (recordingManager->getCurrentLap() > lapConfig.totalLaps)
            {
                Serial.println("All laps completed - stopping recording");
                stopRecording();
            }
        }

        // **OPTIMASI 4: AI classification dengan interval yang tepat**
        if (currentTime - lastClassification >= Config::CLASSIFICATION_INTERVAL)
        {
            updateAIClassification();
            lastClassification = currentTime;
        }
        displayManager->update(); // Ini penting untuk responsivitas visual

       
    }
    catch (...)
    {
        Serial.println("ERROR: Exception in main update loop!");
        // systemMonitor->logError("Main update loop exception");
    }

    lastUpdate = currentTime;
}

void RacingTelemetry::updateAIClassification()
{
    unsigned long startTime = millis();

    try
    {
        int newClassification = classifier->classify(sensorManager->getCurrentData());
        unsigned long executionTime = millis() - startTime;

        // Update classification if changed
        if (newClassification != currentClassification)
        {
            currentClassification = newClassification;
            classificationText = classifier->getClassificationText(currentClassification);

            Serial.printf("AI Classification changed to: %s\n", classificationText.c_str());

            // Handle critical classification
            if (currentClassification == 2)
            { // Critical
                Serial.println("WARNING: AI detected critical engine condition!");
                // systemMonitor->logWarning("AI detected critical condition: " + classificationText);
            }
        }

        classifier->logClassificationResult(sensorManager->getCurrentData(),
                                            currentClassification, executionTime);
    }
    catch (...)
    {
        Serial.println("ERROR: Exception in AI classification!");
        // systemMonitor->logError("AI classification failed");
        currentClassification = 0; // Default to normal
        classificationText = "Error";
    }
}

// void RacingTelemetry::checkSystemHealth()
// {
//     static unsigned long lastHealthCheck = 0;
//     unsigned long currentTime = millis();

//     if (currentTime - lastHealthCheck >= 10000)
//     { // Check every 10 seconds
//         // Check memory usage
//         if (systemMonitor->getMemoryUsagePercent() > 85.0f)
//         {
//             systemMonitor->logWarning("High memory usage detected");
//         }

//         // Check system errors
//         if (systemMonitor->getErrorCount() > 5)
//         {
//             systemMonitor->handleEmergency("Multiple system errors detected");
//         }

//         // Check GPS status during recording
//         if (currentStatus == SystemStatus::RECORDING && !sensorManager->isGPSValid())
//         {
//             systemMonitor->logWarning("GPS signal lost during recording");
//         }

//         lastHealthCheck = currentTime;
//     }
// }

void RacingTelemetry::handleEmergencyCondition(const String &reason)
{
    Serial.printf("EMERGENCY CONDITION: %s\n", reason.c_str());

    // Stop recording if active
    if (currentStatus == SystemStatus::RECORDING)
    {
        Serial.println("Emergency stop - halting recording");
        stopRecording();
    }

    // Set emergency status
    currentStatus = SystemStatus::EMERGENCY;

    // Activate emergency cooling if temperature related
    if (reason.indexOf("overheating") >= 0 || reason.indexOf("temperature") >= 0)
    {
        coolingSystem->emergencyShutdown();
    }

    // Exit menu if in menu
    if (displayManager->isInMenu())
    {
        displayManager->exitMenu();
    }

    // Log emergency
    // systemMonitor->handleEmergency(reason);

    // Show emergency display
    displayManager->showEmergencyMessage(reason);
}

void RacingTelemetry::handleSerialInput()
{
    static String serialCmd = "";

    while (Serial.available())
    {
        char c = Serial.read();
        if (c == '\n' || c == '\r')
        {
            if (serialCmd.length() > 0)
            {
                serialCmd.trim(); // Remove whitespace
                if (serialCmd.length() > 0)
                {
                    Serial.printf("Received command: '%s'\n", serialCmd.c_str());
                    handleSerialCommand(serialCmd);
                }
                serialCmd = "";
            }
        }
        else if (c >= 32 && c <= 126)
        { // Printable characters only
            serialCmd += c;
        }

        // Prevent buffer overflow
        if (serialCmd.length() > 50)
        {
            Serial.println("Command too long - ignored");
            serialCmd = "";
        }
    }
}

void RacingTelemetry::handleSerialCommand(const String &command)
{
    String cmd = command;
    cmd.toUpperCase(); // Make case-insensitive

    if (cmd == "1" || cmd == "START")
    {
        startRecording();
    }
    else if (cmd == "2" || cmd == "TRANSMIT")
    {
        serialActive = true;
        transmitData();
    }
    else if (cmd == "STOP")
    {
        stopRecording();
    }
    else if (cmd == "STATUS")
    {
        printSystemStatus();
    }
    else if (cmd == "MENU")
    {
        enterMenu();
    }
    else if (cmd == "EXIT")
    {
        exitMenu();
    }
    else if (cmd == "COOLING_ON")
    {
        coolingSystem->start();
        Serial.println("Cooling system started via serial command");
    }
    else if (cmd == "COOLING_OFF")
    {
        coolingSystem->stop();
        Serial.println("Cooling system stopped via serial command");
    }
    else if (cmd == "COOLING_STATUS")
    {
        printCoolingStatus();
    }
    else if (cmd == "MEMORY")
    {
        // printMemoryStatus();
    }
    else if (cmd == "GPS")
    {
        printGPSStatus();
    }
    else if (cmd == "SENSORS")
    {
        printSensorStatus();
    }
    else if (cmd == "AI")
    {
        printAIStatus();
    }
    else if (cmd == "HELP")
    {
        printHelpMenu();
    }
    else if (cmd == "RESET")
    {
        performSystemReset();
    }
    else if (cmd == "DEBUG")
    {
        toggleDebugMode();
    }
    else
    {
        // Forward to recording manager for recording-specific commands
        recordingManager->handleSerialCommand(command);
    }
}

void RacingTelemetry::printSystemStatus()
{
    Serial.println("=== SYSTEM STATUS ===");
    Serial.printf("Status: %d (%s)\n", static_cast<int>(currentStatus), getStatusText().c_str());
    Serial.printf("Recording: %s\n", recordingManager->getIsRecording() ? "YES" : "NO");
    Serial.printf("Transmitting: %s\n", recordingManager->getIsTransmitting() ? "YES" : "NO");
    Serial.printf("Current Lap: %d/%d\n", recordingManager->getCurrentLap(), lapConfig.totalLaps);
    Serial.printf("Cooling: %s\n", coolingSystem->isSystemActive() ? "ON" : "OFF");
    Serial.printf("Menu: %s\n", displayManager->isInMenu() ? "ACTIVE" : "INACTIVE");
    Serial.printf("AI Classification: %s\n", classificationText.c_str());
    Serial.printf("System Uptime: %lu ms\n", millis());
    Serial.printf("Free Heap: %d bytes\n", ESP.getFreeHeap());
    // Serial.printf("System Errors: %d\n", systemMonitor->getErrorCount());
}

void RacingTelemetry::printCoolingStatus()
{
    Serial.println("=== COOLING STATUS ===");
    Serial.printf("System: %s\n", coolingSystem->isSystemActive() ? "ON" : "OFF");
    Serial.printf("EWP: %s\n", coolingSystem->isEWPOn() ? "ON" : "OFF");
    Serial.printf("Fan: %s\n", coolingSystem->isFanOn() ? "ON" : "OFF");
    Serial.printf("Cut-off: %s\n", coolingSystem->isCutoffActive() ? "ACTIVE" : "OFF");
    Serial.printf("Temperature: %.1f°C\n", coolingSystem->getCurrentTemp());
    Serial.printf("Fan ON Temp: %.0f°C\n", coolingSystem->getFanOnTemp());
    Serial.printf("Cut-off Temp: %.0f°C\n", coolingSystem->getCutoffTemp());
}

void RacingTelemetry::printMemoryStatus()
{
    Serial.println("=== MEMORY STATUS ===");
    // Serial.printf("Usage: %.1f%%\n", systemMonitor->getMemoryUsagePercent());
    Serial.printf("Free Heap: %d bytes\n", ESP.getFreeHeap());
    Serial.printf("Total Heap: %d bytes\n", ESP.getHeapSize());
    Serial.printf("Min Free Heap: %d bytes\n", ESP.getMinFreeHeap());
    Serial.printf("Max Alloc Heap: %d bytes\n", ESP.getMaxAllocHeap());
    // Serial.printf("System Errors: %d\n", systemMonitor->getErrorCount());
}

void RacingTelemetry::printGPSStatus()
{
    Serial.println("=== GPS STATUS ===");
    Serial.printf("Valid: %s\n", sensorManager->isGPSValid() ? "YES" : "NO");
    if (sensorManager->isGPSValid())
    {
        Serial.printf("Latitude: %.6f\n", sensorManager->getLatitude());
        Serial.printf("Longitude: %.6f\n", sensorManager->getLongitude());
        Serial.printf("Speed: %.1f km/h\n", sensorManager->getSpeed());
        Serial.printf("Satellites: %d\n", sensorManager->getSatelliteCount());
    }
    else
    {
        Serial.println("No GPS fix available");
    }
}

void RacingTelemetry::printSensorStatus()
{
    const SensorData &data = sensorManager->getCurrentData();
    Serial.println("=== SENSOR STATUS ===");
    Serial.printf("AFR: %.1f\n", data.afr);
    Serial.printf("RPM: %.0f\n", data.rpm);
    Serial.printf("Temperature: %.1f°C\n", data.temp);
    Serial.printf("TPS: %.1f%%\n", data.tps);
    Serial.printf("MAP: %.1f kPa\n", data.map_value);
    Serial.printf("Incline: %.1f°\n", data.incline);
    Serial.printf("Stroke: %.1f mm\n", data.stroke);
    Serial.printf("Timestamp: %lu\n", data.timestamp);
}

void RacingTelemetry::printAIStatus()
{
    Serial.println("=== AI STATUS ===");
    Serial.printf("Current Classification: %d (%s)\n", currentClassification, classificationText.c_str());
    Serial.printf("Training Data Size: %d\n", Config::TRAIN_DATA_SIZE);
    Serial.printf("K-Value: %d\n", Config::K_VALUE);
    Serial.printf("Last Classification: %lu ms ago\n", millis() - lastClassification);
}

void RacingTelemetry::printHelpMenu()
{
    Serial.println("=== AVAILABLE COMMANDS ===");
    Serial.println("1 or START     - Start recording");
    Serial.println("2 or TRANSMIT  - Transmit data");
    Serial.println("STOP           - Stop recording");
    Serial.println("STATUS         - Show system status");
    Serial.println("MENU           - Enter menu");
    Serial.println("EXIT           - Exit menu");
    Serial.println("COOLING_ON     - Start cooling system");
    Serial.println("COOLING_OFF    - Stop cooling system");
    Serial.println("COOLING_STATUS - Show cooling status");
    Serial.println("MEMORY         - Show memory status");
    Serial.println("GPS            - Show GPS status");
    Serial.println("SENSORS        - Show sensor readings");
    Serial.println("AI             - Show AI classification status");
    Serial.println("HELP           - Show this help menu");
    Serial.println("RESET          - Perform system reset");
    Serial.println("DEBUG          - Toggle debug mode");
}

void RacingTelemetry::performSystemReset()
{
    Serial.println("=== PERFORMING SYSTEM RESET ===");

    // Stop all operations
    if (currentStatus == SystemStatus::RECORDING)
    {
        stopRecording();
    }

    // Reset all components
    currentStatus = SystemStatus::IDLE;
    currentClassification = 0;
    classificationText = "Normal";
    lastUpdate = millis();
    lastClassification = millis();

    // Reset display
    displayManager->exitMenu();
    displayManager->forceUpdate();

    // Reset system monitor
    // systemMonitor->recoverFromError();

    Serial.println("System reset completed");
}

void RacingTelemetry::toggleDebugMode()
{
    static bool debugMode = false;
    debugMode = !debugMode;

    Serial.printf("Debug mode: %s\n", debugMode ? "ON" : "OFF");

    // Could implement debug features here
    if (debugMode)
    {
        Serial.println("Enhanced logging enabled");
    }
    else
    {
        Serial.println("Normal logging mode");
    }
}

String RacingTelemetry::getStatusText() const
{
    switch (currentStatus)
    {
    case SystemStatus::IDLE:
        return "IDLE";
    case SystemStatus::RECORDING:
        return "RECORDING";
    case SystemStatus::TRANSMITTING:
        return "TRANSMITTING";
    case SystemStatus::MENU:
        return "MENU";
    case SystemStatus::ERROR:
        return "ERROR";
    case SystemStatus::EMERGENCY:
        return "EMERGENCY";
    default:
        return "UNKNOWN";
    }
}

void RacingTelemetry::startRecording()
{
    if (currentStatus == SystemStatus::RECORDING)
    {
        Serial.println("Already recording!");
        return;
    }

    if (currentStatus == SystemStatus::TRANSMITTING)
    {
        Serial.println("Cannot start recording while transmitting!");
        return;
    }

    if (currentStatus == SystemStatus::EMERGENCY)
    {
        Serial.println("Cannot start recording in emergency state!");
        return;
    }

    // Pre-recording checks
    if (!sensorManager->isGPSValid() && lapConfig.mode == LapDetectionMode::GPS_RETURN_TO_START)
    {
        Serial.println("WARNING: No GPS fix for GPS-based lap detection!");
    }

    if (coolingSystem->getCurrentTemp() > 100.0f)
    {
        Serial.println("WARNING: High temperature detected before recording!");
    }

    // Start recording
    currentStatus = SystemStatus::RECORDING;
    recordingManager->startRecording();
    displayManager->exitMenu();

    // Auto-start cooling system if not active
    if (!coolingSystem->isSystemActive())
    {
        coolingSystem->start();
        Serial.println("Auto-started cooling system for recording");
    }

    Serial.printf("Racing Telemetry: Recording started (%d laps, mode %d)\n",
                  lapConfig.totalLaps, static_cast<int>(lapConfig.mode));
}

void RacingTelemetry::stopRecording()
{
    if (currentStatus != SystemStatus::RECORDING)
    {
        Serial.println("Not currently recording!");
        return;
    }

    currentStatus = SystemStatus::IDLE;
    recordingManager->stopRecording();

    Serial.println("Racing Telemetry: Recording stopped");

    // Show completion display
    displayManager->forceUpdate(); // Atau gunakan showSystemStatus()

}

void RacingTelemetry::transmitData()
{
    if (currentStatus == SystemStatus::RECORDING)
    {
        Serial.println("Cannot transmit while recording!");
        return;
    }

    if (currentStatus == SystemStatus::TRANSMITTING)
    {
        Serial.println("Already transmitting!");
        return;
    }

    if (currentStatus == SystemStatus::EMERGENCY)
    {
        Serial.println("Cannot transmit in emergency state!");
        return;
    }

    currentStatus = SystemStatus::TRANSMITTING;
    recordingManager->transmitAllData();
    currentStatus = SystemStatus::IDLE;

    Serial.println("Racing Telemetry: Data transmission completed");
}

void RacingTelemetry::enterMenu()
{
    if (currentStatus == SystemStatus::RECORDING ||
        currentStatus == SystemStatus::TRANSMITTING)
    {
        Serial.println("Cannot enter menu during recording or transmission!");
        return;
    }

    if (currentStatus == SystemStatus::EMERGENCY)
    {
        Serial.println("Cannot enter menu in emergency state!");
        return;
    }

    currentStatus = SystemStatus::MENU;
    displayManager->enterMenu();
    Serial.println("Entered menu system");
}

void RacingTelemetry::exitMenu()
{
    if (currentStatus == SystemStatus::MENU)
    {
        currentStatus = SystemStatus::IDLE;
        displayManager->exitMenu();
        Serial.println("Exited menu system");
    }
}
