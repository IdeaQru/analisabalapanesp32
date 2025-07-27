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

/**
 * @brief Main Racing Telemetry System Class
 * 
 * This is the primary controller class that orchestrates all subsystems
 * of the ESP32 Racing Telemetry system including:
 * - AI-powered engine condition classification
 * - Intelligent cooling system management
 * - Multi-mode lap detection and recording
 * - Animated TFT display with real-time data visualization
 * - Comprehensive sensor data collection and analysis
 * - Robust error handling and system monitoring
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
    // SystemMonitor* systemMonitor;       // System health monitoring
    
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
    
    // === PRIVATE METHODS ===
    
    /**
     * @brief Updates AI classification system
     * Performs KNN classification on current sensor data and handles
     * classification changes and critical condition detection
     */
    void updateAIClassification();
    
    /**
     * @brief Performs comprehensive system health checks
     * Monitors memory usage, error counts, GPS status, and other
     * critical system parameters
     */
    void checkSystemHealth();
    
    /**
     * @brief Handles emergency conditions
     * @param reason Description of the emergency condition
     * Automatically stops recording, activates emergency cooling,
     * and switches to emergency display mode
     */
    void handleEmergencyCondition(const String& reason);
    
    /**
     * @brief Processes individual serial commands
     * @param command The command string to process
     * Handles all supported serial commands including recording control,
     * system status queries, and diagnostic commands
     */
    void handleSerialCommand(const String& command);
    
    // === SERIAL COMMAND HANDLERS ===
    void printSystemStatus();           // Print comprehensive system status
    void printCoolingStatus();          // Print cooling system details
    void printMemoryStatus();           // Print memory usage information
    void printGPSStatus();              // Print GPS status and coordinates
    void printSensorStatus();           // Print all sensor readings
    void printAIStatus();               // Print AI classification status
    void printHelpMenu();               // Print available commands
    void performSystemReset();          // Perform soft system reset
    void toggleDebugMode();             // Toggle debug logging mode
    
    /**
     * @brief Converts system status enum to readable string
     * @return Human-readable status description
     */
    String getStatusText() const;

public:
    // === CONSTRUCTOR & DESTRUCTOR ===
    
    /**
     * @brief Default constructor
     * Initializes all member variables to safe default values
     */
    RacingTelemetry();
    bool serialActive = false;
    /**
     * @brief Destructor
     * Ensures proper cleanup of resources (singletons don't need deletion)
     */
    ~RacingTelemetry();
    
    // === SYSTEM LIFECYCLE ===
    
    /**
     * @brief Initializes the complete racing telemetry system
     * Performs ordered initialization of all subsystems:
     * 1. Hardware component initialization
     * 2. Sensor calibration and setup
     * 3. Display system startup with animations
     * 4. System health verification
     * 5. Configuration loading and validation
     */
    void initialize();
    
    /**
     * @brief Main system update loop
     * Coordinates updates across all subsystems in proper order:
     * - Input handling (buttons, serial)
     * - Sensor data collection
     * - Cooling system management
     * - Recording operations
     * - AI classification
     * - Display updates
     * - System monitoring
     */
    void update();
    
    /**
     * @brief Handles incoming serial communication
     * Processes serial input character by character, building complete
     * commands and forwarding them for execution
     */
    void handleSerialInput();
    
    // === SYSTEM STATE GETTERS ===
    
    /**
     * @brief Get current system operating status
     * @return SystemStatus enum value
     */
    SystemStatus getStatus() const { return currentStatus; }
    
    /**
     * @brief Get current AI classification result
     * @return Classification ID (0=Normal, 1=Maintenance, 2=Critical)
     */
    int getCurrentClassification() const { return currentClassification; }
    
    /**
     * @brief Get human-readable classification text
     * @return String description of current classification
     */
    String getClassificationText() const { return classificationText; }
    
    // === RECORDING CONTROL ===
    
    /**
     * @brief Starts data recording session
     * Performs pre-recording validation checks and starts:
     * - Sensor data logging
     * - Lap detection
     * - Automatic cooling system activation
     * - Recording status display
     */
    void startRecording();
    
    /**
     * @brief Stops current recording session
     * Safely terminates recording and displays completion summary
     */
    void stopRecording();
    
    /**
     * @brief Transmits all recorded data via serial
     * Sends complete dataset in CSV format with progress indication
     */
    void transmitData();
    
    // === MENU SYSTEM CONTROL ===
    
    /**
     * @brief Enters interactive menu system
     * Switches to menu mode for system configuration and settings
     */
    void enterMenu();
    
    /**
     * @brief Exits menu system
     * Returns to main telemetry display
     */
    void exitMenu();
    
    // === CONFIGURATION ACCESS ===
    
    /**
     * @brief Get lap detection configuration
     * @return Reference to lap configuration object
     */
    LapConfiguration& getLapConfig() { return lapConfig; }
    
    /**
     * @brief Get display configuration
     * @return Reference to display configuration object
     */
    DisplayConfiguration& getDisplayConfig() { return displayConfig; }
    
    // === SINGLETON PATTERN ===
    
    /**
     * @brief Get singleton instance of RacingTelemetry
     * @return Reference to the single system instance
     * 
     * Implements thread-safe singleton pattern for global system access
     */
    static RacingTelemetry& getInstance() {
        static RacingTelemetry instance;
        return instance;
    }
    
    // === SYSTEM INFORMATION ===
    
    /**
     * @brief Get system version information
     * @return Version string
     */
    static String getVersion() { return "Racing Telemetry OOP v2.0"; }
    
    /**
     * @brief Get system build date
     * @return Build timestamp
     */
    static String getBuildDate() { return __DATE__ " " __TIME__; }
    
    /**
     * @brief Get supported features list
     * @return Comma-separated feature list
     */
    static String getSupportedFeatures() { 
        return "AI-Classification,Multi-Lap-Detection,Animated-Display,Intelligent-Cooling,Real-time-Monitoring"; 
    }
};

// === GLOBAL SYSTEM ACCESS MACROS ===
#define TELEMETRY_SYSTEM RacingTelemetry::getInstance()
#define SYSTEM_STATUS TELEMETRY_SYSTEM.getStatus()
#define AI_CLASSIFICATION TELEMETRY_SYSTEM.getCurrentClassification()

#endif // RACING_TELEMETRY_H
