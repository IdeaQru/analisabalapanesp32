#include "RacingTelemetry.h"

RacingTelemetry::RacingTelemetry()
    : classifier(nullptr), coolingSystem(nullptr), sensorManager(nullptr),
      displayManager(nullptr), buttonHandler(nullptr), recordingManager(nullptr),
      currentStatus(SystemStatus::IDLE), lastUpdate(0), lastClassification(0),
      currentClassification(0), classificationText("Normal"), serialActive(false),
      apiEndpoint("https://http://47.237.23.149:7187/api/telemetry"),
      apiKey("your-api-key-here"), deviceId("racing-001"),
      ssid("YOUR_WIFI_SSID"), password("YOUR_WIFI_PASSWORD")
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

        Serial.println("✓ System Monitor initialized");
    }
    catch (...)
    {
        Serial.println("ERROR: Exception during component initialization!");
        return;
    }

    // Initialize WiFi
    Serial.println("Initializing WiFi connection...");
    if (connectToWiFi())
    {
        Serial.println("✓ WiFi connection established");
    }
    else
    {
        Serial.println("⚠ WiFi connection failed - API features disabled");
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
        // Sensor update setiap 20ms
        sensorManager->update();
        sensorManager->updateGPS();

        // **OPTIMASI 3: Cooling system dengan interval yang wajar**
        static unsigned long lastCoolingUpdate = 0;
        if (currentTime - lastCoolingUpdate >= 20)
        { // Cooling update setiap 20ms
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

            // **TAMBAHAN: Auto-send ke API selama recording**
        }

        // **OPTIMASI 4: AI classification dengan interval yang tepat**
        if (currentTime - lastClassification >= Config::CLASSIFICATION_INTERVAL)
        {
            updateAIClassification();
            lastClassification = currentTime;
        }
        static unsigned long lastAPISend = 0;
        if (currentTime - lastAPISend >= 1000)
        { // 1000ms = 1 detik
            // Kirim data ke API hanya jika WiFi connected
            if (WiFi.status() == WL_CONNECTED)
            {
                sendCurrentDataToAPI();
                lastAPISend = currentTime;
            }
            else
            {
                Serial.println("WiFi not connected - skipping API send");
                lastAPISend = currentTime; // Reset timer even if failed
            }
        }
        displayManager->update(); // Ini penting untuk responsivitas visual
    }
    catch (...)
    {
        Serial.println("ERROR: Exception in main update loop!");
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
            }
        }

        classifier->logClassificationResult(sensorManager->getCurrentData(),
                                            currentClassification, executionTime);
    }
    catch (...)
    {
        Serial.println("ERROR: Exception in AI classification!");
        currentClassification = 0; // Default to normal
        classificationText = "Error";
    }
}

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
    else if (cmd == "3" || cmd == "SEND_API")
    {
        sendCurrentDataToAPI();
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
        printMemoryStatus();
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
    else if (cmd == "WIFI_STATUS")
    {
        printWiFiStatus();
    }
    else if (cmd == "API_TEST")
    {
        testAPIConnection();
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

// === API METHODS IMPLEMENTATION ===

bool RacingTelemetry::connectToWiFi()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        return true;
    }

    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid, password);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20)
    {
        delay(500);
        Serial.print(".");
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("\nWiFi connected!");
        Serial.printf("IP address: %s\n", WiFi.localIP().toString().c_str());
        return true;
    }
    else
    {
        Serial.println("\nWiFi connection failed!");
        return false;
    }
}

bool RacingTelemetry::sendDataToAPI(const String &jsonData)
{
    if (!connectToWiFi())
    {
        Serial.println("Cannot send to API: No WiFi connection");
        return false;
    }

    HTTPClient http;
    http.begin(apiEndpoint);

    // Set headers - hanya Content-Type
    http.addHeader("Content-Type", "application/json");

    // Set timeout
    http.setTimeout(5000); // 5 seconds timeout

    // Send POST request
    int httpCode = http.POST(jsonData);
    String response = http.getString();

    handleAPIResponse(httpCode, response);

    http.end();
    return (httpCode == 200 || httpCode == 201);
}

String RacingTelemetry::prepareTelemetryJSON()
{
    const SensorData &data = sensorManager->getCurrentData();

    // Create JSON document
    DynamicJsonDocument doc(2048);

    // Device information
    doc["device_id"] = deviceId;
    doc["timestamp"] = millis();
    doc["system_status"] = getStatusText();
    doc["lap_number"] = recordingManager->getCurrentLap();

    // Sensor data
    JsonObject sensors = doc.createNestedObject("sensors");
    sensors["afr"] = data.afr;
    sensors["rpm"] = data.rpm;
    sensors["temperature"] = data.temp;
    sensors["tps"] = data.tps;
    sensors["map_value"] = data.map_value;
    sensors["incline"] = data.incline;
    sensors["stroke"] = data.stroke;

    // GPS data
    if (sensorManager->isGPSValid())
    {
        JsonObject gps = doc.createNestedObject("gps");
        gps["latitude"] = sensorManager->getLatitude();
        gps["longitude"] = sensorManager->getLongitude();
        gps["speed"] = sensorManager->getSpeed();
        gps["satellites"] = sensorManager->getSatelliteCount();
    }

    // AI Classification
    JsonObject ai = doc.createNestedObject("ai_classification");
    ai["classification"] = currentClassification;
    ai["classification_text"] = classificationText;

    // Cooling system status
    JsonObject cooling = doc.createNestedObject("cooling");
    cooling["system_active"] = coolingSystem->isSystemActive();
    cooling["fan_on"] = coolingSystem->isFanOn();
    cooling["ewp_on"] = coolingSystem->isEWPOn();
    cooling["current_temp"] = coolingSystem->getCurrentTemp();
    cooling["cutoff_active"] = coolingSystem->isCutoffActive();

    // System health
    JsonObject system = doc.createNestedObject("system_health");
    system["free_heap"] = ESP.getFreeHeap();
    system["uptime"] = millis();
    system["wifi_rssi"] = WiFi.RSSI();

    String jsonString;
    serializeJson(doc, jsonString);
    return jsonString;
}

void RacingTelemetry::handleAPIResponse(int httpCode, const String &response)
{
    if (httpCode == 200 || httpCode == 201)
    {
        Serial.println("✓ Data sent to API successfully");
        Serial.printf("Response: %s\n", response.c_str());
    }
    else if (httpCode > 0)
    {
        Serial.printf("✗ API Error - HTTP %d: %s\n", httpCode, response.c_str());
    }
    else
    {
        Serial.printf("✗ Connection Error: %d\n", httpCode);
    }
}

void RacingTelemetry::sendCurrentDataToAPI()
{
    if (currentStatus == SystemStatus::EMERGENCY)
    {
        Serial.println("Cannot send to API in emergency state!");
        return;
    }

    Serial.println("Preparing telemetry data for API...");
    String jsonData = prepareTelemetryJSON();

    Serial.printf("JSON Size: %d bytes\n", jsonData.length());

    if (sendDataToAPI(jsonData))
    {
        Serial.println("Telemetry data sent to API successfully!");
    }
    else
    {
        Serial.println("Failed to send telemetry data to API!");
    }
}

// === PRINT STATUS METHODS ===

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
    Serial.printf("WiFi Status: %s\n", WiFi.status() == WL_CONNECTED ? "CONNECTED" : "DISCONNECTED");
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
    Serial.printf("Free Heap: %d bytes\n", ESP.getFreeHeap());
    Serial.printf("Total Heap: %d bytes\n", ESP.getHeapSize());
    Serial.printf("Min Free Heap: %d bytes\n", ESP.getMinFreeHeap());
    Serial.printf("Max Alloc Heap: %d bytes\n", ESP.getMaxAllocHeap());
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

void RacingTelemetry::printWiFiStatus()
{
    Serial.println("=== WIFI STATUS ===");
    Serial.printf("Status: %s\n", WiFi.status() == WL_CONNECTED ? "CONNECTED" : "DISCONNECTED");
    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.printf("SSID: %s\n", WiFi.SSID().c_str());
        Serial.printf("IP: %s\n", WiFi.localIP().toString().c_str());
        Serial.printf("RSSI: %d dBm\n", WiFi.RSSI());
        Serial.printf("Gateway: %s\n", WiFi.gatewayIP().toString().c_str());
    }
}

void RacingTelemetry::printHelpMenu()
{
    Serial.println("=== AVAILABLE COMMANDS ===");
    Serial.println("1 or START     - Start recording");
    Serial.println("2 or TRANSMIT  - Transmit data");
    Serial.println("3 or SEND_API  - Send current data to API");
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
    Serial.println("WIFI_STATUS    - Show WiFi connection status");
    Serial.println("API_TEST       - Test API connection");
    Serial.println("HELP           - Show this help menu");
    Serial.println("RESET          - Perform system reset");
    Serial.println("DEBUG          - Toggle debug mode");
}

void RacingTelemetry::testAPIConnection()
{
    Serial.println("Testing API connection...");

    // Create minimal test payload
    DynamicJsonDocument testDoc(512);
    testDoc["device_id"] = deviceId;
    testDoc["test"] = true;
    testDoc["timestamp"] = millis();

    String testJson;
    serializeJson(testDoc, testJson);

    if (sendDataToAPI(testJson))
    {
        Serial.println("✓ API connection test successful!");
    }
    else
    {
        Serial.println("✗ API connection test failed!");
    }
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

// === SYSTEM CONTROL METHODS ===

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
    displayManager->forceUpdate();
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
