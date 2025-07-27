#ifndef BUTTON_HANDLER_H
#define BUTTON_HANDLER_H

#include "DataStructures.h"

class ButtonHandler
{
private:
    // Button states untuk 3 button system
    bool lastBtnRec, lastBtnCursor, lastBtnTx;

    // Press timing tracking
    unsigned long btnRecPressTime;
    unsigned long btnCursorPressTime;
    unsigned long btnTxPressTime;

    // Press state tracking
    bool btnRecPressed;
    bool btnCursorPressed;
    bool btnTxPressed;
    bool isRecLongPressTriggered;
    bool isCursorMenuTriggered;

    // Responsiveness settings
    unsigned long lastResponsiveUpdate;
    bool responsiveModeEnabled;
    static const unsigned long RESPONSIVE_UPDATE_INTERVAL = 10; // 10ms ultra responsive

    // Debouncing
    static const unsigned long DEBOUNCE_DELAY = 50; // Increased for stability
    unsigned long lastDebounceTime[3];              // For 3 buttons
    bool lastStableState[3];                        // Track stable states

    // Private methods - Menu handling
    void handleMenuNavigation();
    void handleMenuSelection();

    // Private methods - Button processing
    bool readInputOnlyPin(int pin) const;
    bool debounceButton(int buttonIndex, bool currentState);
    bool validateButtonPress(unsigned long pressDuration, unsigned long minTime, unsigned long maxTime = 0);

    // Private methods - Debug support
    void testSystemReferences();
    void handleStuckButtons(unsigned long currentTime);

public:
    // Constructor & Destructor
    ButtonHandler();
    ~ButtonHandler();

    // Core functionality
    void initialize();
    void update();

    // Button state methods - Real-time button reading
    bool isRecordButtonPressed() const;
    bool isCursorButtonPressed() const;
    bool isTransmitButtonPressed() const;

    // Press duration methods - Get current press duration
    unsigned long getRecordButtonPressDuration() const;
    unsigned long getCursorButtonPressDuration() const;
    unsigned long getTransmitButtonPressDuration() const;

    // Button status methods - Combined status info
    bool isAnyButtonPressed() const;
    String getButtonStatusString() const;

    // Responsiveness control
    void setResponsiveMode(bool enabled);
    bool isResponsiveModeEnabled() const;
    void setUpdateInterval(unsigned long interval);

    // Static methods untuk akses global
    static ButtonHandler &getInstance()
    {
        static ButtonHandler instance;
        return instance;
    }

    // **PERBAIKAN: Debug getters tanpa static variables**
    unsigned long getUpdateCallCount() const;
    unsigned long getLastUpdateTime() const;
    bool getSystemReferencesValid() const;

    // **HAPUS: Static variables yang menyebabkan linker error**
    // static unsigned long updateCallCount;
    // static unsigned long lastUpdateTime;
    // static bool systemReferencesValid;
};

// External time options declaration
extern const TimeOption timeOptions[34];

#endif // BUTTON_HANDLER_H
