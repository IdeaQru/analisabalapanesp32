#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include "DataStructures.h"

class DisplayManager {
private:
    Adafruit_ST7735* tft;
    DisplayConfiguration* config;
    MenuState currentMenu;
    int menuSelection;
    bool inMenu;
    unsigned long lastUpdate;
    
    // Animation variables
    unsigned long lastAnimationUpdate;
    int animationFrame;
    float scrollOffset;
    bool blinkState;
    unsigned long lastBlink;
    
    // UI Layout constants
    static const int TITLE_HEIGHT = 14;
    static const int STATUS_BAR_HEIGHT = 20;
    static const int MENU_ITEM_HEIGHT = 12;
    static const int SENSOR_LINE_HEIGHT = 11;
    static const int ANIMATION_SPEED = 150;
    static const int BLINK_SPEED = 500;
    static const int SCROLL_SPEED = 2;
    
    // Color scheme
    static const uint16_t COLOR_HEADER = ST77XX_CYAN;
    static const uint16_t COLOR_MENU_SELECTED = ST77XX_YELLOW;
    static const uint16_t COLOR_MENU_NORMAL = ST77XX_WHITE;
    static const uint16_t COLOR_SENSOR_NORMAL = ST77XX_WHITE;
    static const uint16_t COLOR_SENSOR_WARNING = ST77XX_YELLOW;
    static const uint16_t COLOR_SENSOR_CRITICAL = ST77XX_RED;
    static const uint16_t COLOR_GPS_GOOD = ST77XX_GREEN;
    static const uint16_t COLOR_GPS_BAD = ST77XX_RED;
    static const uint16_t COLOR_STATUS_BAR = ST77XX_MAGENTA;
    
    // Private drawing methods - DISESUAIKAN DENGAN .CPP
    void clearScreen();
    void drawAnimatedHeader(const String& title, uint16_t color = COLOR_HEADER);
    void drawProgressBarAnimated(int x, int y, int width, int height, float percentage, uint16_t color);
    void drawLoadingSpinner(int x, int y, uint16_t color);
    void drawPulsingDot(int x, int y, uint16_t color);
    void drawScrollingText(const String& text, int x, int y, int maxWidth, uint16_t color);
    void drawBlinkingText(const String& text, int x, int y, uint16_t color);
    
    // Menu drawing methods - SESUAI IMPLEMENTASI .CPP
    void drawMainMenuAnimated();
    void drawLapConfigMenu();        // BUKAN drawLapConfigMenuAnimated
    void drawDisplayConfigMenu();    // BUKAN drawDisplayConfigMenuAnimated
    void drawCoolingConfigMenu();    // BUKAN drawCoolingConfigMenuAnimated
    void drawLapCountMenu();         // BUKAN drawLapCountMenuAnimated
    void drawDistanceSetMenu();      // BUKAN drawDistanceSetMenuAnimated
    void drawTimeSetMenu();          // BUKAN drawTimeSetMenuAnimated
    
    // Status display methods - SESUAI IMPLEMENTASI .CPP
    void drawMainDisplay();          // BUKAN drawMainDisplayAnimated
    void drawRecordingDisplay();     // BUKAN drawRecordingDisplayAnimated
    void drawTransmissionDisplay();  // BUKAN drawTransmissionDisplayAnimated
    void drawEmergencyDisplay();     // BUKAN drawEmergencyDisplayAnimated
    
    // Utility methods - SESUAI IMPLEMENTASI .CPP
    void drawCenteredText(const String& text, int y, uint16_t color, int textSize = 1);
    void drawMenuItem(const String& text, int index, int yPos, bool selected, bool animated = true);
    void drawStatusBar(int y);       // BUKAN drawStatusBarAnimated
    void drawTemperatureIndicator(float temp, int x, int y);  // BUKAN drawTemperatureIndicatorAnimated
    void drawGPSStatus(int x, int y); // BUKAN drawGPSStatusAnimated
    void drawSystemHealthBar(int x, int y, int width);
    
    // Animation helpers
    void updateAnimations();
    float easeInOut(float t);
    uint16_t interpolateColor(uint16_t color1, uint16_t color2, float ratio);

public:
    DisplayManager();
    ~DisplayManager();
    
    void initialize();
    void update();
    void setConfiguration(DisplayConfiguration* config);
    
    // Menu control
    void enterMenu();
    void exitMenu();
    void setCurrentMenu(MenuState menu) { currentMenu = menu; }
    void setMenuSelection(int selection) { menuSelection = selection; }
    
    // Getters
    bool isInMenu() const { return inMenu; }
    MenuState getCurrentMenu() const { return currentMenu; }
    int getMenuSelection() const { return menuSelection; }
    
    // Display control - SESUAI IMPLEMENTASI .CPP
    void showStartupScreen();        // BUKAN showStartupScreenAnimated
    void showEmergencyMessage(const String& message);
    void showSystemStatus();
    void forceUpdate() { lastUpdate = 0; }
    
    // Animation control
    void resetAnimations();
    void setAnimationSpeed(int speed) { /* Implementation for dynamic speed */ }
    
    // Static methods untuk akses global
    static DisplayManager& getInstance() {
        static DisplayManager instance;
        return instance;
    }
};

#endif // DISPLAY_MANAGER_H
