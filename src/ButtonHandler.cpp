#include "ButtonHandler.h"
#include "RacingTelemetry.h"
#include "DisplayManager.h"
#include "CoolingSystem.h"

// Static debug flag
static bool debugMode = false;

ButtonHandler::ButtonHandler() 
    : lastBtnRec(LOW), lastBtnCursor(LOW), lastBtnTx(LOW),
      btnRecPressTime(0), btnCursorPressTime(0), btnTxPressTime(0),
      btnRecPressed(false), btnCursorPressed(false), btnTxPressed(false),
      isRecLongPressTriggered(false), isCursorMenuTriggered(false),
      lastResponsiveUpdate(0), responsiveModeEnabled(false) {
    
    for (int i = 0; i < 3; i++) {
        lastDebounceTime[i] = 0;
        lastStableState[i] = LOW;
    }
    
    Serial.println("ButtonHandler constructor called (PULLDOWN mode)");
}

ButtonHandler::~ButtonHandler() {
    Serial.println("ButtonHandler destroyed");
}

void ButtonHandler::initialize() {
    Serial.println("=== Button Handler Initializing (PULLDOWN System) ===");
    
    pinMode(Config::BTN_REC, INPUT_PULLDOWN);      // Pin 15 - Recording
    pinMode(Config::BTN_CURSOR, INPUT_PULLDOWN);   // Pin 23 - Navigation
    pinMode(Config::BTN_TX, INPUT_PULLDOWN);       // Pin 22 - Select Only
    
    delay(300);
    
    lastBtnRec = LOW;
    lastBtnCursor = LOW;
    lastBtnTx = LOW;
    
    for (int i = 0; i < 3; i++) {
        lastStableState[i] = LOW;
        lastDebounceTime[i] = 0;
    }
    
    btnRecPressed = false;
    btnCursorPressed = false;
    btnTxPressed = false;
    isRecLongPressTriggered = false;
    isCursorMenuTriggered = false;
    lastResponsiveUpdate = millis();
    if (digitalRead(Config::BTN_TX) == HIGH)
    {
        realTime = true; 
    }else
    {
        realTime = false;
    }
    
    Serial.println("=== Button Configuration (PULLDOWN MODE) ===");
    Serial.printf("REC Button: Pin %d - Hold 2s: Start/Stop Recording\n", Config::BTN_REC);
    Serial.printf("CURSOR Button: Pin %d - Press: Enter Menu/Navigation\n", Config::BTN_CURSOR);
    Serial.printf("SELECT Button: Pin %d - Press: Select/Confirm Only\n", Config::BTN_TX);
    
    Serial.println("=== Button Handler Ready ===");
}

void ButtonHandler::update() {
    unsigned long currentTime = millis();
    
    // **PERBAIKAN: Hapus responsive mode restriction yang bermasalah**
    // if (responsiveModeEnabled && (currentTime - lastResponsiveUpdate < 25)) {
    //     return;
    // }
    lastResponsiveUpdate = currentTime;
    
    // **PERBAIKAN: Read raw button states - TIDAK PAKAI DEBOUNCING DULU**
    bool btnRec = digitalRead(Config::BTN_REC) == HIGH;
    bool btnCursor = digitalRead(Config::BTN_CURSOR) == HIGH;
    bool btnTx = digitalRead(Config::BTN_TX) == HIGH;
    
    // **PERBAIKAN: Debug hanya saat ada perubahan**
    static bool lastBtnRecState = false, lastBtnCursorState = false, lastBtnTxState = false;
    if (btnRec != lastBtnRecState || btnCursor != lastBtnCursorState || btnTx != lastBtnTxState) {
        Serial.printf("BUTTON CHANGE: REC=%d, CURSOR=%d, SELECT=%d\n", btnRec, btnCursor, btnTx);
        lastBtnRecState = btnRec;
        lastBtnCursorState = btnCursor;
        lastBtnTxState = btnTx;
    }
    
    // Get system references
    RacingTelemetry& system = RacingTelemetry::getInstance();
    DisplayManager& display = DisplayManager::getInstance();
    
    // **PERBAIKAN: System state debug hanya saat berubah**
    static SystemStatus lastSystemStatus = SystemStatus::IDLE;
    static bool lastInMenuState = false;
    
    SystemStatus currentSystemStatus = system.getStatus();
    bool currentInMenuState = display.isInMenu();
    
    if (currentSystemStatus != lastSystemStatus || currentInMenuState != lastInMenuState) {
        Serial.printf("SYSTEM CHANGE: Status=%d, InMenu=%s\n", 
                      static_cast<int>(currentSystemStatus), 
                      currentInMenuState ? "YES" : "NO");
        lastSystemStatus = currentSystemStatus;
        lastInMenuState = currentInMenuState;
    }
    
    // === BTN_REC Logic - 2 second hold for recording ===
    if (btnRec == HIGH && lastBtnRec == LOW) {
        btnRecPressTime = currentTime;
        btnRecPressed = true;
        isRecLongPressTriggered = false;
        Serial.println("REC: PRESSED - Need 2s for activation");
    }
    
    if (btnRec == HIGH && btnRecPressed && !isRecLongPressTriggered) {
        unsigned long pressDuration = currentTime - btnRecPressTime;
        
        if (pressDuration >= 2000) { // 2 seconds
            isRecLongPressTriggered = true;
            Serial.printf("REC: ACTIVATED after %lums\n", pressDuration);
            
            if (system.getStatus() != SystemStatus::TRANSMITTING) {
                if (system.getStatus() == SystemStatus::RECORDING) {
                    system.stopRecording();
                    Serial.println("=== RECORDING STOPPED ===");
                } else {
                    if (display.isInMenu()) display.exitMenu();
                    system.startRecording();
                    Serial.println("=== RECORDING STARTED ===");
                }
            }
        }
    }
    
    if (btnRec == LOW && lastBtnRec == HIGH) {
        unsigned long pressDuration = currentTime - btnRecPressTime;
        btnRecPressed = false;
        if (pressDuration < 2000) {
            Serial.printf("REC: Short press (%lums) - No action\n", pressDuration);
        }
    }

    // === BTN_CURSOR Logic - SIMPLIFIED Menu Entry/Navigation ===
    if (btnCursor == HIGH && lastBtnCursor == LOW) {
        btnCursorPressTime = currentTime;
        btnCursorPressed = true;
        Serial.println("CURSOR: PRESSED");
    }
    
    if (btnCursor == LOW && lastBtnCursor == HIGH && btnCursorPressed) {
        unsigned long pressDuration = currentTime - btnCursorPressTime;
        btnCursorPressed = false;
        
        // **PERBAIKAN: Lebih toleran untuk press duration**
        if (pressDuration >= 1 && pressDuration < 3000) { 
            Serial.printf("CURSOR: Valid press (%lums)\n", pressDuration);
            
            bool currentlyInMenu = display.isInMenu();
            SystemStatus currentStatus = system.getStatus();
            
            if (currentlyInMenu) {
                // **SUDAH DALAM MENU = Navigation antar item**
                Serial.println("Menu navigation...");
                handleMenuNavigation();
                Serial.println("=== MENU NAVIGATION DONE ===");
            } else {
                // **BELUM DALAM MENU = Masuk ke menu**
                if (currentStatus == SystemStatus::IDLE) {
                    Serial.println("Entering menu...");
                    
                    display.enterMenu();
                    
                    // **Verifikasi langsung tanpa delay**
                    if (display.isInMenu()) {
                        Serial.println("=== MENU ENTERED ===");
                    } else {
                        Serial.println("ERROR: Menu entry failed");
                    }
                    
                } else {
                    Serial.printf("Cannot enter menu - Status: %d\n", static_cast<int>(currentStatus));
                }
            }
        } else {
            Serial.printf("CURSOR: Invalid press duration (%lums)\n", pressDuration);
        }
    }

    // === BTN_TX Logic - SIMPLIFIED Selection ===
    if (btnTx == HIGH && lastBtnTx == LOW) {
        btnTxPressTime = currentTime;
        btnTxPressed = true;
        Serial.println("SELECT: PRESSED");
    }
    
    if (btnTx == LOW && lastBtnTx == HIGH && btnTxPressed) {
        unsigned long pressDuration = currentTime - btnTxPressTime;
        btnTxPressed = false;
        
        // **PERBAIKAN: Lebih toleran untuk press duration**
        if (pressDuration >= 1 && pressDuration < 3000) {
            Serial.printf("SELECT: Valid press (%lums)\n", pressDuration);
            
            bool currentlyInMenu = display.isInMenu();
            
            if (currentlyInMenu) {
                Serial.println("Menu selection...");
                handleMenuSelection();
                Serial.println("=== MENU SELECTION DONE ===");
            } else {
                // **SELECT juga bisa masuk menu**
                SystemStatus currentStatus = system.getStatus();
                
                if (currentStatus == SystemStatus::IDLE) {
                    Serial.println("SELECT entering menu...");
                    
                    display.enterMenu();
                    
                    if (display.isInMenu()) {
                        Serial.println("=== MENU ENTERED VIA SELECT ===");
                    } else {
                        Serial.println("ERROR: SELECT menu entry failed");
                    }
                } else {
                    Serial.printf("SELECT: No action - Status: %d\n", static_cast<int>(currentStatus));
                }
            }
        } else {
            Serial.printf("SELECT: Invalid press duration (%lums)\n", pressDuration);
        }
    }
    
    // Update last button states
    lastBtnRec = btnRec;
    lastBtnCursor = btnCursor;
    lastBtnTx = btnTx;
}

void ButtonHandler::handleMenuNavigation() {
    DisplayManager& display = DisplayManager::getInstance();
    
    int currentSelection = display.getMenuSelection();
    MenuState currentMenu = display.getCurrentMenu();
    int maxSelection = 0;
    
    switch (currentMenu) {
        case MenuState::MAIN:
            maxSelection = 2;
            break;
        case MenuState::LAP_CONFIG:
            maxSelection = 4;
            break;
        case MenuState::LAP_COUNT:
            maxSelection = 11;
            break;
        case MenuState::DISTANCE_SET:
            maxSelection = 11;
            break;
        case MenuState::TIME_SET:
            maxSelection = 35;
            break;
        default:
            maxSelection = 2;
            break;
    }
    
    currentSelection = (currentSelection + 1) % maxSelection;
    display.setMenuSelection(currentSelection);
    
    Serial.printf("NAV: menu=%d, selection=%d/%d\n", 
                  static_cast<int>(currentMenu), currentSelection, maxSelection - 1);
}

void ButtonHandler::handleMenuSelection() {
    RacingTelemetry& system = RacingTelemetry::getInstance();
    DisplayManager& display = DisplayManager::getInstance();
    
    MenuState currentMenu = display.getCurrentMenu();
    int selection = display.getMenuSelection();
    
    Serial.printf("SELECT: menu=%d, selection=%d\n", 
                  static_cast<int>(currentMenu), selection);
    
    switch (currentMenu) {
        case MenuState::MAIN: {
            switch (selection) {
                case 0: // [R] Record
                    if (system.getStatus() == SystemStatus::RECORDING) {
                        system.stopRecording();
                        Serial.println("Recording stopped via menu");
                    } else {
                        system.startRecording();
                        Serial.println("Recording started via menu");
                    }
                    display.exitMenu();
                    break;
                case 1: // [L] Lap Setup
                    display.setCurrentMenu(MenuState::LAP_CONFIG);
                    display.setMenuSelection(0);
                    Serial.println("Entering Lap Config");
                    break;
            }
            break;
        }
        
        case MenuState::LAP_CONFIG: {
            LapConfiguration& lapConfig = system.getLapConfig();
            switch (selection) {
                case 0: // Mode selection
                    {
                        int currentMode = static_cast<int>(lapConfig.mode);
                        currentMode = (currentMode + 1) % 3;
                        lapConfig.mode = static_cast<LapDetectionMode>(currentMode);
                        Serial.printf("Lap mode changed to: %d\n", currentMode);
                    }
                    break;
                case 1: // Lap count
                    display.setCurrentMenu(MenuState::LAP_COUNT);
                    display.setMenuSelection(0);
                    Serial.println("Entering Lap Count selection");
                    break;
                case 2: // Distance/Time setting
                    if (lapConfig.mode == LapDetectionMode::DISTANCE_BASED) {
                        display.setCurrentMenu(MenuState::DISTANCE_SET);
                        display.setMenuSelection(0);
                        Serial.println("Entering Distance Set");
                    } else if (lapConfig.mode == LapDetectionMode::TIME_BASED) {
                        display.setCurrentMenu(MenuState::TIME_SET);
                        display.setMenuSelection(0);
                        Serial.println("Entering Time Set");
                    }
                    break;
                case 3: // Back
                    display.setCurrentMenu(MenuState::MAIN);
                    display.setMenuSelection(0);
                    Serial.println("Returning to main menu");
                    break;
            }
            break;
        }
        
        case MenuState::LAP_COUNT: {
            if (selection < 10) {
                LapConfiguration& lapConfig = system.getLapConfig();
                lapConfig.totalLaps = selection + 1;
                Serial.printf("Lap count set to: %d\n", lapConfig.totalLaps);
                display.setCurrentMenu(MenuState::LAP_CONFIG);
                display.setMenuSelection(0);
            } else if (selection == 10) { // Back
                display.setCurrentMenu(MenuState::LAP_CONFIG);
                display.setMenuSelection(0);
                Serial.println("Returning to lap config");
            }
            break;
        }
        
        case MenuState::DISTANCE_SET: {
            if (selection < 10) {
                LapConfiguration& lapConfig = system.getLapConfig();
                lapConfig.targetDistance = (selection + 1) * 100.0f;
                Serial.printf("Distance set to: %.0fm\n", lapConfig.targetDistance);
                display.setCurrentMenu(MenuState::LAP_CONFIG);
                display.setMenuSelection(0);
            } else if (selection == 10) { // Back
                display.setCurrentMenu(MenuState::LAP_CONFIG);
                display.setMenuSelection(0);
                Serial.println("Returning to lap config");
            }
            break;
        }
        
        case MenuState::TIME_SET: {
            extern const TimeOption timeOptions[34];
            if (selection < 34) {
                LapConfiguration& lapConfig = system.getLapConfig();
                lapConfig.targetTime = timeOptions[selection].seconds;
                Serial.printf("Time set to: %ds (%s)\n", 
                              lapConfig.targetTime, timeOptions[selection].displayText);
                display.setCurrentMenu(MenuState::LAP_CONFIG);
                display.setMenuSelection(0);
            } else if (selection == 34) { // Back
                display.setCurrentMenu(MenuState::LAP_CONFIG);
                display.setMenuSelection(0);
                Serial.println("Returning to lap config");
            }
            break;
        }
        
        default:
            Serial.println("Menu selection not implemented for this menu");
            break;
    }
}

// **PERBAIKAN: Debouncing yang benar**
bool ButtonHandler::debounceButton(int buttonIndex, bool currentState) {
    unsigned long currentTime = millis();
    
    // Jika state berubah, reset timer
    if (currentState != lastStableState[buttonIndex]) {
        lastDebounceTime[buttonIndex] = currentTime;
    }
    
    // Jika sudah cukup lama stable, update state
    if ((currentTime - lastDebounceTime[buttonIndex]) > DEBOUNCE_DELAY) {
        lastStableState[buttonIndex] = currentState;
    }
    
    return lastStableState[buttonIndex];
}

// Implementation of remaining methods from header
bool ButtonHandler::readInputOnlyPin(int pin) const {
    return digitalRead(pin) == HIGH;
}

bool ButtonHandler::validateButtonPress(unsigned long pressDuration, unsigned long minTime, unsigned long maxTime) {
    return pressDuration >= minTime && (maxTime == 0 || pressDuration < maxTime);
}

bool ButtonHandler::isRecordButtonPressed() const {
    return digitalRead(Config::BTN_REC) == HIGH;
}

bool ButtonHandler::isCursorButtonPressed() const {
    return digitalRead(Config::BTN_CURSOR) == HIGH;
}

bool ButtonHandler::isTransmitButtonPressed() const {
    return digitalRead(Config::BTN_TX) == HIGH;
}

bool ButtonHandler::isAnyButtonPressed() const {
    return isRecordButtonPressed() || isCursorButtonPressed() || isTransmitButtonPressed();
}

String ButtonHandler::getButtonStatusString() const {
    String status = "Buttons: ";
    status += isRecordButtonPressed() ? "REC " : "rec ";
    status += isCursorButtonPressed() ? "CUR " : "cur ";
    status += isTransmitButtonPressed() ? "SEL" : "sel";
    return status;
}

unsigned long ButtonHandler::getRecordButtonPressDuration() const {
    if (btnRecPressed) {
        return millis() - btnRecPressTime;
    }
    return 0;
}

unsigned long ButtonHandler::getCursorButtonPressDuration() const {
    if (btnCursorPressed) {
        return millis() - btnCursorPressTime;
    }
    return 0;
}

unsigned long ButtonHandler::getTransmitButtonPressDuration() const {
    if (btnTxPressed) {
        return millis() - btnTxPressTime;
    }
    return 0;
}

void ButtonHandler::setResponsiveMode(bool enabled) {
    responsiveModeEnabled = enabled;
}

bool ButtonHandler::isResponsiveModeEnabled() const {
    return responsiveModeEnabled;
}

void ButtonHandler::setUpdateInterval(unsigned long interval) {
    // Implementation if needed
}

void ButtonHandler::testSystemReferences() {
    // Implementation if needed
}

unsigned long ButtonHandler::getUpdateCallCount() const {
    return millis() / 50;
}

unsigned long ButtonHandler::getLastUpdateTime() const {
    return lastResponsiveUpdate;
}

bool ButtonHandler::getSystemReferencesValid() const {
    try {
        RacingTelemetry& system = RacingTelemetry::getInstance();
        DisplayManager& display = DisplayManager::getInstance();
        CoolingSystem& cooling = CoolingSystem::getInstance();
        return true;
    } catch (...) {
        return false;
    }
}
