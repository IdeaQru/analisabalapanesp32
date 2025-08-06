#include "DisplayManager.h"
#include "SensorManager.h"
#include "CoolingSystem.h"
#include "KNNClassifier.h"
#include "RacingTelemetry.h"
#include "RecordingManager.h"
#include "SystemMonitor.h"

// Time options array
const TimeOption timeOptions[34] = {
    {10, "10s"}, {15, "15s"}, {20, "20s"}, {30, "30s"}, {45, "45s"}, {60, "1m"}, 
    {90, "1m30s"}, {120, "2m"}, {150, "2m30s"}, {180, "3m"}, {210, "3m30s"}, 
    {240, "4m"}, {270, "4m30s"}, {300, "5m"}, {330, "5m30s"}, {360, "6m"}, 
    {390, "6m30s"}, {420, "7m"}, {450, "7m30s"}, {480, "8m"}, {510, "8m30s"}, 
    {540, "9m"}, {570, "9m30s"}, {600, "10m"}, {630, "10m30s"}, {660, "11m"}, 
    {690, "11m30s"}, {720, "12m"}, {750, "12m30s"}, {780, "13m"}, {810, "13m30s"}, 
    {840, "14m"}, {870, "14m30s"}, {900, "15m"}
};

DisplayManager::DisplayManager()
    : tft(nullptr), config(nullptr), currentMenu(MenuState::MAIN),
      menuSelection(0), inMenu(false), lastUpdate(0),
      lastAnimationUpdate(0), animationFrame(0), scrollOffset(0.0f),
      blinkState(false), lastBlink(0)
{
}

DisplayManager::~DisplayManager()
{
    delete tft;
}

void DisplayManager::initialize()
{
    tft = new Adafruit_ST7735(Config::TFT_CS, Config::TFT_DC, Config::TFT_RST);
    tft->initR(INITR_BLACKTAB);
    tft->setRotation(1); // Landscape 160x128
    tft->fillScreen(ST77XX_BLACK);

    // Initialize animation variables
    resetAnimations();

    Serial.println("=== TFT Display Manager Initialized ===");
    Serial.printf("Display: %dx%d\n", Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT);
}

void DisplayManager::update()
{
    if (!config) return;

    unsigned long currentTime = millis();

    // **OPTIMASI - Tingkatkan minimum refresh rate untuk stabilitas**
    unsigned long minRefreshRate = max((unsigned long)config->refreshRate, 200UL);
    if (currentTime - lastUpdate < minRefreshRate) return;

    // **OPTIMASI - Update animasi lebih jarang**
    updateAnimations();

    RacingTelemetry &system = RacingTelemetry::getInstance();

    if (inMenu)
    {
        switch (currentMenu)
        {
        case MenuState::MAIN:
            drawMainMenuAnimated();
            break;
        case MenuState::LAP_CONFIG:
            drawLapConfigMenu();
            break;
        case MenuState::LAP_COUNT:
            drawLapCountMenu();
            break;
        case MenuState::DISTANCE_SET:
            drawDistanceSetMenu();
            break;
        case MenuState::TIME_SET:
            drawTimeSetMenu();
            break;
        }
    }
    else
    {
        switch (system.getStatus())
        {
        case SystemStatus::RECORDING:
            drawRecordingDisplay();
            break;
        case SystemStatus::TRANSMITTING:
            drawTransmissionDisplay();
            break;
        case SystemStatus::IDLE:
        default:
            drawMainDisplay();
            break;
        }
    }

    lastUpdate = currentTime;
}

void DisplayManager::updateAnimations()
{
    unsigned long currentTime = millis();

    if (currentTime - lastAnimationUpdate >= max((unsigned long)ANIMATION_SPEED, 300UL))
    {
        animationFrame = (animationFrame + 1) % 16;
        scrollOffset += SCROLL_SPEED;
        if (scrollOffset > 200) scrollOffset = 0;
        lastAnimationUpdate = currentTime;
    }

    if (currentTime - lastBlink >= max((unsigned long)BLINK_SPEED, 600UL))
    {
        blinkState = !blinkState;
        lastBlink = currentTime;
    }
}

void DisplayManager::clearScreen()
{
    tft->fillScreen(ST77XX_BLACK);
}

void DisplayManager::drawAnimatedHeader(const String &title, uint16_t color)
{
    tft->setTextSize(1);
    tft->setTextColor(color);

    int16_t x1, y1;
    uint16_t textWidth, textHeight;
    tft->getTextBounds(title, 0, 0, &x1, &y1, &textWidth, &textHeight);

    int centerX = (Config::SCREEN_WIDTH - textWidth) / 2;

    tft->setCursor(centerX, Config::MARGIN_Y + 2 - 5);
    tft->println(title);

    int lineY = Config::MARGIN_Y - 2 - 10;

    // for (int i = 0; i < 6; i++)
    // {
    //     int x = (Config::SCREEN_WIDTH / 6) * i;
    //     int wave = (i % 2 == 0) ? 2 : 1;
    //     if (animationFrame % 8 < 4)
    //     {
    //         tft->drawPixel(x, lineY + wave, color);
    //     }
    // }

    tft->fillRect(0, 0, 3, 2, color);
    tft->fillRect(Config::SCREEN_WIDTH - 3, 0, 3, 2, color);
}

void DisplayManager::drawProgressBarAnimated(int x, int y, int width, int height, float percentage, uint16_t color)
{
    tft->drawRect(x, y, width, height, ST77XX_WHITE);

    int fillWidth = (width - 2) * (percentage / 100.0f);
    if (fillWidth > 0)
    {
        tft->fillRect(x + 1, y + 1, fillWidth, height - 2, color);
    }

    if (animationFrame % 16 < 8)
    {
        tft->drawRect(x - 1, y - 1, width + 2, height + 2, color);
    }
}

void DisplayManager::drawLoadingSpinner(int x, int y, uint16_t color)
{
    static char spinChars[] = {'|', '/', '-', '\\'};
    tft->setTextSize(1);
    tft->setTextColor(color);
    tft->setCursor(x, y);
    tft->print(spinChars[animationFrame % 4]);
}

void DisplayManager::drawPulsingDot(int x, int y, uint16_t color)
{
    int radius = (animationFrame % 8 < 4) ? 3 : 2;
    tft->fillCircle(x, y, radius, color);
}

void DisplayManager::drawScrollingText(const String &text, int x, int y, int maxWidth, uint16_t color)
{
    tft->setTextSize(1);
    tft->setTextColor(color);

    int textWidth = text.length() * 6;
    if (textWidth <= maxWidth)
    {
        tft->setCursor(x, y);
        tft->print(text);
    }
    else
    {
        static int scrollPos = 0;
        if (animationFrame % 8 == 0)
        {
            scrollPos += 6;
            if (scrollPos > textWidth) scrollPos = 0;
        }

        int startX = x - scrollPos;
        if (startX < x + maxWidth)
        {
            tft->setCursor(startX, y);
            tft->print(text);
        }
    }
}

void DisplayManager::drawBlinkingText(const String &text, int x, int y, uint16_t color)
{
    if (blinkState)
    {
        tft->setTextSize(1);
        tft->setTextColor(color);
        tft->setCursor(x, y);
        tft->print(text);
    }
}

void DisplayManager::drawMainMenuAnimated()
{
    static bool needsRedraw = true;
    static int lastMenuSelection = -1;

    if (needsRedraw || lastMenuSelection != menuSelection)
    {
        clearScreen();
        needsRedraw = false;
        lastMenuSelection = menuSelection;
    }

    drawAnimatedHeader("RACING TELEMETRY");

    String menuItems[] = {
        "[R] Start Recording", 
        "[L] Lap Setup"
    };

    int startY = TITLE_HEIGHT + 15;
    for (int i = 0; i < 2; i++)
    {
        bool selected = (i == menuSelection);
        drawMenuItem(menuItems[i], i, startY + (i * MENU_ITEM_HEIGHT * 2), selected, true);

        if (selected)
        {
            tft->setTextSize(1);
            tft->setTextColor(COLOR_MENU_SELECTED);
            tft->setCursor(Config::MARGIN_X - 18, startY + (i * MENU_ITEM_HEIGHT * 2));
            tft->print(">");
        }
    }

    // drawSystemHealthBar(Config::SCREEN_WIDTH - 25, TITLE_HEIGHT + 50, 20);
    drawStatusBar(Config::SCREEN_HEIGHT - STATUS_BAR_HEIGHT);
}

// **PERBAIKAN UTAMA - Main Display dengan Clear Area yang Proper**
void DisplayManager::drawMainDisplay()
{
    unsigned long currentTime = millis();
    static unsigned long lastDataUpdate = 0;
    static unsigned long lastFullClear = 0;
    
    // **OPTIMASI 1 - Full clear hanya jika diperlukan (kurangi flicker)**
    bool needsFullClear = false;
    if (currentTime - lastFullClear > 300) { // 5 detik sekali saja
        needsFullClear = true;
        lastFullClear = currentTime;
    }
    
    // **OPTIMASI 2 - Selective clearing berdasarkan data changes**
    static String lastClassificationText = "";
    static float lastTemp = -999;
    static bool lastGPSValid = false;
    
    RacingTelemetry &system = RacingTelemetry::getInstance();
    SensorManager &sensors = SensorManager::getInstance();
    CoolingSystem &cooling = CoolingSystem::getInstance();
    KNNClassifier &classifier = KNNClassifier::getInstance();
    
    // Check for significant data changes
    String currentClassificationText = system.getClassificationText();
    float currentTemp = cooling.getCurrentTemp();
    bool currentGPSValid = sensors.isGPSValid();
    
    bool dataChanged = (currentClassificationText != lastClassificationText) ||
                      (abs(currentTemp - lastTemp) > 1.0f) ||
                      (currentGPSValid != lastGPSValid);
    
    if (needsFullClear || dataChanged || (currentTime - lastDataUpdate > 300)) {
        clearScreen();
        lastDataUpdate = currentTime;
        
        // Update comparison values
        lastClassificationText = currentClassificationText;
        lastTemp = currentTemp;
        lastGPSValid = currentGPSValid;
    }

    int yPos = Config::MARGIN_Y;

    // Header - hanya draw jika perlu
    String title = "TELEMETRY LIVE";
    drawAnimatedHeader(title);
    yPos = TITLE_HEIGHT + 5;

    // **OPTIMASI 3 - Smart content area clearing**
    int contentStartY = yPos;
    int contentHeight = Config::SCREEN_HEIGHT - STATUS_BAR_HEIGHT - contentStartY;
    
    // Hanya clear content area jika ada data change
    if (dataChanged || needsFullClear) {
        tft->fillRect(0, contentStartY, Config::SCREEN_WIDTH, contentHeight, ST77XX_BLACK);
    }

    // AI Classification - dengan change detection
    if (config->showClassification)
    {
        static String lastDisplayedClassification = "";
        if (currentClassificationText != lastDisplayedClassification || needsFullClear) {
            // Clear hanya area classification
            tft->fillRect(Config::MARGIN_X, yPos, Config::SCREEN_WIDTH - Config::MARGIN_X * 2, 12, ST77XX_BLACK);
            
            tft->setTextSize(1);
            tft->setTextColor(classifier.getClassificationColor(system.getCurrentClassification()));
            tft->setCursor(Config::MARGIN_X, yPos);
            tft->printf("AI: %s", currentClassificationText.c_str());
            
            lastDisplayedClassification = currentClassificationText;
        }
        yPos += 12;
    }

    // GPS Status - dengan smart update
    if (config->showGPS)
    {
        static bool lastGPSDisplayState = false;
        if (currentGPSValid != lastGPSDisplayState || needsFullClear) {
            drawGPSStatus(Config::MARGIN_X, yPos);
            lastGPSDisplayState = currentGPSValid;
        }
        yPos += 22;
    }

    // Cooling system - dengan temperature threshold
    if (config->showCoolingStatus)
    {
        static float lastDisplayedTemp = -999;
        if (abs(currentTemp - lastDisplayedTemp) > 0.5f || needsFullClear) {
            // Clear temperature area
            tft->fillRect(Config::MARGIN_X, yPos, Config::SCREEN_WIDTH - Config::MARGIN_X * 2, 18, ST77XX_BLACK);
            drawTemperatureIndicator(currentTemp, Config::MARGIN_X, yPos);
            lastDisplayedTemp = currentTemp;
        }
        yPos += 18;
    }

    // **PERBAIKAN UTAMA - Sensor data batch update untuk tampil bersamaan**
    if (config->showSensors)
    {
        const SensorData &data = sensors.getCurrentData();
        
        static float lastAFR = -1, lastMAP = -1, lastTPS = -1, lastRPM = -1;
        static unsigned long lastSensorDisplayUpdate = 0;
        
        // **KUNCI: Deteksi perubahan SEMUA sensor sekaligus**
        bool sensorDataChanged = (abs(data.afr - lastAFR) > 0.1f) ||
                                (abs(data.map_value - lastMAP) > 1.0f) ||
                                (abs(data.tps - lastTPS) > 1.0f) ||
                                (abs(data.rpm - lastRPM) > 50.0f);
        
        // **BATCH UPDATE - Update SEMUA data sensor bersamaan**
        // Interval yang sama untuk semua sensor: 300ms
        if (sensorDataChanged || 
            (currentTime - lastSensorDisplayUpdate > 300) || 
            needsFullClear) {
            
            // **Clear seluruh sensor area sekaligus**
            tft->fillRect(Config::MARGIN_X, yPos, Config::SCREEN_WIDTH - Config::MARGIN_X * 2, 50, ST77XX_BLACK);

            // **RENDER SEMUA DATA SENSOR DALAM SATU BATCH**
            tft->setTextSize(1);
            tft->setTextColor(COLOR_SENSOR_NORMAL);
            
            // **Baris pertama: AFR & MAP**
            tft->setCursor(Config::MARGIN_X, yPos);
            tft->printf("AFR:%.1f MAP:%.0f", data.afr, data.map_value);
            
            // **Progress bar AFR**
            float afrPercent = (data.afr / 18.0f) * 100.0f;
            if (afrPercent > 100) afrPercent = 100;
            // drawProgressBarAnimated(Config::MARGIN_X + 80, yPos + 12, 40, 4, afrPercent, ST77XX_GREEN);
            
            // **Baris kedua: TPS & RPM**
            int tempYPos = yPos + 20;
            tft->setCursor(Config::MARGIN_X, tempYPos);
            tft->printf("TPS:%.0f%% RPM:%.0f", data.tps, data.rpm);
            
            // **Progress bar RPM**
            float rpmPercent = (data.rpm / 8000.0f) * 100.0f;
            if (rpmPercent > 100) rpmPercent = 100;
            // drawProgressBarAnimated(Config::MARGIN_X + 80, tempYPos + 12, 40, 4, rpmPercent, ST77XX_CYAN);
            
            // **Update SEMUA nilai comparison bersamaan**
            lastAFR = data.afr;
            lastMAP = data.map_value;
            lastTPS = data.tps;
            lastRPM = data.rpm;
            lastSensorDisplayUpdate = currentTime;
        }
        yPos += 40;
    }

    // **OPTIMASI 5 - Data flow indicator yang lebih smooth**
    static unsigned long lastIndicatorUpdate = 0;
    if (currentTime - lastIndicatorUpdate > 100) { // Update setiap 100ms
        // Clear area indicator dulu
        tft->fillRect(Config::SCREEN_WIDTH - 5, TITLE_HEIGHT + 10, 2, 8, ST77XX_BLACK);
        
        if (animationFrame % 16 < 8)
        {
            tft->fillRect(Config::SCREEN_WIDTH - 5, TITLE_HEIGHT + 10, 2, 8, ST77XX_CYAN);
        }
        lastIndicatorUpdate = currentTime;
    }

    // Status bar - update dengan interval
    // static unsigned long lastStatusUpdate = 0;
    // if (currentTime - lastStatusUpdate > 500 || needsFullClear) { // Update setiap 500ms
    //     drawStatusBar(Config::SCREEN_HEIGHT - STATUS_BAR_HEIGHT);
    //     lastStatusUpdate = currentTime;
    // }
}

// **PERBAIKAN - Recording display dengan clear area yang proper**
void DisplayManager::drawRecordingDisplay()
{
    unsigned long currentTime = millis();
    static unsigned long lastRecordingUpdate = 0;
    static unsigned long lastSensorCycleUpdate = 0;
    static int sensorDisplayMode = 0; // 0=Cooling, 1=Engine, 2=GPS+AI
    
    // **PERBAIKAN - Clear screen setiap 100ms**
    if (currentTime - lastRecordingUpdate > 100) {
        clearScreen();
        lastRecordingUpdate = currentTime;
    }

    // **SENSOR CYCLING - Ganti mode setiap 2 detik**
    if (currentTime - lastSensorCycleUpdate > 2000) {
        sensorDisplayMode = (sensorDisplayMode + 1) % 3;
        lastSensorCycleUpdate = currentTime;
    }

    String recText = blinkState ? "[REC]" : " REC ";
    drawAnimatedHeader(recText, ST77XX_RED);

    RecordingManager &recording = RecordingManager::getInstance();
    CoolingSystem &cooling = CoolingSystem::getInstance();
    SensorManager &sensors = SensorManager::getInstance();
    RacingTelemetry &system = RacingTelemetry::getInstance();
    KNNClassifier &classifier = KNNClassifier::getInstance();

    int yPos = TITLE_HEIGHT + 8;

    // **PERBAIKAN - Clear content area**
    int contentStartY = yPos;
    int contentHeight = Config::SCREEN_HEIGHT - STATUS_BAR_HEIGHT - contentStartY - 20;
    tft->fillRect(0, contentStartY, Config::SCREEN_WIDTH, contentHeight, ST77XX_BLACK);

    // Large lap counter - SELALU TAMPIL
    tft->setTextSize(2);
    tft->setTextColor(ST77XX_WHITE);
    tft->setCursor(Config::MARGIN_X, yPos);
    tft->printf("LAP %d", recording.getCurrentLap());

    // Recording time - SELALU TAMPIL
    unsigned long recordTime = (millis() / 1000) % 3600;
    int minutes = recordTime / 60;
    int seconds = recordTime % 60;

    tft->setTextSize(1);
    tft->setCursor(Config::MARGIN_X + 70, yPos + 5);
    tft->setTextColor(ST77XX_YELLOW);
    tft->printf("%02d:%02d", minutes, seconds);

    // **SENSOR CYCLE INDICATOR**
    tft->setTextColor(ST77XX_CYAN);
    tft->setCursor(Config::MARGIN_X + 70, yPos + 15);
    String modeNames[] = {"COOLING", "ENGINE", "GPS+AI"};
    tft->printf("<%s>", modeNames[sensorDisplayMode].c_str());

    yPos += 35;

    // **ALTERNATING SENSOR DISPLAY BERDASARKAN MODE**
    tft->setTextSize(1);
    
    switch(sensorDisplayMode) {
        case 0: // COOLING SYSTEM MODE
            {
                // **Temperature**
                tft->setTextColor(ST77XX_WHITE);
                tft->setCursor(Config::MARGIN_X, yPos);
                tft->printf("Temp: %.1fC", cooling.getCurrentTemp());
                
                // **Fan Status**
                tft->setTextColor(cooling.isFanOn() ? ST77XX_RED : ST77XX_GREEN);
                tft->setCursor(Config::MARGIN_X + 75, yPos);
                tft->printf("Fan: %s", cooling.isFanOn() ? "ON" : "OFF");
                yPos += 12;
                
                // **EWP Status**
                tft->setTextColor(cooling.isEWPOn() ? ST77XX_GREEN : ST77XX_RED);
                tft->setCursor(Config::MARGIN_X, yPos);
                tft->printf("EWP: %s", cooling.isEWPOn() ? "ON" : "OFF");
                
                // **Cutoff Status**
                tft->setTextColor(cooling.isCutoffActive() ? ST77XX_RED : ST77XX_GREEN);
                tft->setCursor(Config::MARGIN_X + 75, yPos);
                tft->printf("Cut: %s", cooling.isCutoffActive() ? "ACT" : "OFF");
                yPos += 12;
                
                // **Cooling Status Text**
                tft->setTextColor(cooling.getStatusColor());
                tft->setCursor(Config::MARGIN_X, yPos);
                tft->printf("Status: %s", cooling.getStatusText().c_str());
            }
            break;
            
        case 1: // ENGINE SENSOR MODE
            {
                const SensorData &data = sensors.getCurrentData();
                
                // **Baris 1: AFR & MAP**
                tft->setTextColor(COLOR_SENSOR_NORMAL);
                tft->setCursor(Config::MARGIN_X, yPos);
                tft->printf("AFR: %.1f", data.afr);
                
                tft->setCursor(Config::MARGIN_X + 70, yPos);
                tft->printf("MAP: %.0f", data.map_value);
                yPos += 12;
                
                // **Baris 2: TPS & RPM**
                tft->setCursor(Config::MARGIN_X, yPos);
                tft->printf("TPS: %.0f%%", data.tps);
                
                tft->setCursor(Config::MARGIN_X + 70, yPos);
                tft->printf("RPM: %.0f", data.rpm);
                yPos += 12;
                
                // **Progress Bars untuk Engine Data**
                // AFR Progress (Target: 14.7)
                float afrPercent = (data.afr / 18.0f) * 100.0f;
                if (afrPercent > 100) afrPercent = 100;
                drawProgressBarAnimated(Config::MARGIN_X, yPos, 60, 4, afrPercent, ST77XX_GREEN);
                
                // RPM Progress (Max: 8000)
                float rpmPercent = (data.rpm / 8000.0f) * 100.0f;
                if (rpmPercent > 100) rpmPercent = 100;
                drawProgressBarAnimated(Config::MARGIN_X + 70, yPos, 60, 4, rpmPercent, ST77XX_CYAN);
            }
            break;
            
        case 2: // GPS + AI MODE
            {
                // **GPS Speed dan Status**
                if (sensors.isGPSValid())
                {
                    float speed = sensors.getSpeed();
                    tft->setTextColor(ST77XX_CYAN);
                    tft->setCursor(Config::MARGIN_X, yPos);
                    tft->printf("Speed: %.1f km/h", speed);
                    
                    tft->setCursor(Config::MARGIN_X + 75, yPos);
                    tft->printf("Sat: %d", sensors.getSatelliteCount());
                    yPos += 12;
                    
                    // **GPS Coordinates**
                    tft->setTextColor(ST77XX_WHITE);
                    tft->setCursor(Config::MARGIN_X, yPos);
                    tft->printf("Lat: %.4f", sensors.getLatitude());
                    yPos += 12;
                    
                    tft->setCursor(Config::MARGIN_X, yPos);
                    tft->printf("Lng: %.4f", sensors.getLongitude());
                }
                else
                {
                    tft->setTextColor(ST77XX_RED);
                    tft->setCursor(Config::MARGIN_X, yPos);
                    tft->printf("GPS: SEARCHING...");
                    yPos += 12;
                    
                    tft->setCursor(Config::MARGIN_X, yPos);
                    tft->printf("No Satellite Fix");
                }
                yPos += 12;
                
                // **AI Classification**
                tft->setTextColor(classifier.getClassificationColor(system.getCurrentClassification()));
                tft->setCursor(Config::MARGIN_X, yPos);
                tft->printf("AI: %s", system.getClassificationText().c_str());
            }
            break;
    }

    yPos += 20;

    // Simple stop button - SELALU TAMPIL
    int buttonX = Config::MARGIN_X;
    int buttonY = yPos;
    int buttonWidth = 100;
    int buttonHeight = 12;

    if (blinkState)
    {
        tft->fillRect(buttonX - 2, buttonY - 2, buttonWidth + 4, buttonHeight + 4, ST77XX_RED);
        tft->setTextColor(ST77XX_WHITE);
    }
    else
    {
        tft->drawRect(buttonX - 2, buttonY - 2, buttonWidth + 4, buttonHeight + 4, ST77XX_RED);
        tft->setTextColor(ST77XX_RED);
    }

    tft->setCursor(buttonX, buttonY);
    tft->print("[SELECT] STOP REC");

    // Recording indicator - SELALU TAMPIL
    if (animationFrame % 12 < 6)
    {
        tft->fillCircle(Config::SCREEN_WIDTH - 10, TITLE_HEIGHT + 5, 2, ST77XX_RED);
    }

    // **STATUS BAR dengan Mode Indicator**
    int statusY = Config::SCREEN_HEIGHT - STATUS_BAR_HEIGHT;
    tft->setTextSize(1);

    if (blinkState)
    {
        tft->setTextColor(ST77XX_RED);
        tft->setCursor(Config::MARGIN_X, statusY);
        tft->printf("* REC | Mode:%s *", modeNames[sensorDisplayMode].c_str());
    }
    else
    {
        tft->setTextColor(ST77XX_YELLOW);
        tft->setCursor(Config::MARGIN_X, statusY);
        tft->printf("Mode: %s [2s cycle]", modeNames[sensorDisplayMode].c_str());
    }

    tft->setTextColor(ST77XX_YELLOW);
    tft->setCursor(Config::MARGIN_X, statusY + 10);
    tft->println("Pin36:STOP | Pin21 2s:STOP");
}


void DisplayManager::drawTransmissionDisplay()
{
    static bool needsRedraw = true;
    static float transmissionProgress = 0;

    if (needsRedraw)
    {
        clearScreen();
        needsRedraw = false;
    }

    drawAnimatedHeader("TRANSMITTING", ST77XX_BLUE);

    int centerY = Config::SCREEN_HEIGHT / 2;

    tft->setTextSize(1);
    tft->setTextColor(ST77XX_WHITE);
    tft->setCursor(Config::MARGIN_X, centerY - 20);
    tft->print("Sending Data...");

    transmissionProgress += 1.0f;
    if (transmissionProgress > 100) transmissionProgress = 0;

    drawProgressBarAnimated(Config::MARGIN_X, centerY,
                            Config::SCREEN_WIDTH - Config::MARGIN_X * 2, 8,
                            transmissionProgress, ST77XX_CYAN);

    drawLoadingSpinner(Config::SCREEN_WIDTH / 2 - 8, centerY + 20, ST77XX_BLUE);

    tft->setTextSize(1);
    tft->setTextColor(ST77XX_WHITE);
    tft->setCursor(Config::MARGIN_X, centerY + 12);
    tft->printf("Progress: %.0f%%", transmissionProgress);
}


void DisplayManager::drawLapConfigMenu()
{
    static bool needsRedraw = true;
    static int lastSelection = -1;

    
        clearScreen();
        needsRedraw = false;
        lastSelection = menuSelection;
    

    drawAnimatedHeader("LAP CONFIG", ST77XX_CYAN);

    RacingTelemetry &system = RacingTelemetry::getInstance();
    LapConfiguration &lapConfig = system.getLapConfig();

    int startY = TITLE_HEIGHT + 10;
    String modes[] = {"GPS Return", "Distance", "Time Based"};

    for (int i = 0; i < 4; i++)
    {
        tft->setTextSize(1);
        tft->setCursor(Config::MARGIN_X, startY + (i * MENU_ITEM_HEIGHT * 1.5));

        if (i == menuSelection)
        {
            tft->setTextColor(COLOR_MENU_SELECTED);
            tft->setCursor(Config::MARGIN_X - 8, startY + (i * MENU_ITEM_HEIGHT * 1.5));
            tft->print(">");
        }
        else
        {
            tft->setTextColor(COLOR_MENU_NORMAL);
        }

        tft->setCursor(Config::MARGIN_X, startY + (i * MENU_ITEM_HEIGHT * 1.5));

        switch (i)
        {
        case 0:
            tft->printf("Mode: %s", modes[static_cast<int>(lapConfig.mode)].c_str());
            break;
        case 1:
            tft->printf("Laps: %d", lapConfig.totalLaps);
            break;
        case 2:
            if (lapConfig.mode == LapDetectionMode::DISTANCE_BASED)
            {
                tft->printf("Distance: %.0fm", lapConfig.targetDistance);
            }
            else if (lapConfig.mode == LapDetectionMode::TIME_BASED)
            {
                tft->printf("Time: %ds", lapConfig.targetTime);
            }
            else
            {
                tft->print("GPS Mode Active");
            }
            break;
        case 3:
            tft->print("Back");
            break;
        }
    }

    drawStatusBar(Config::SCREEN_HEIGHT - STATUS_BAR_HEIGHT);
}

void DisplayManager::drawLapCountMenu()
{
    static bool needsRedraw = true;
    static int lastSelection = -1;

    if (needsRedraw || lastSelection != menuSelection)
    {
        clearScreen();
        needsRedraw = false;
        lastSelection = menuSelection;
    }

    drawAnimatedHeader("LAP COUNT", ST77XX_GREEN);

    int startY = TITLE_HEIGHT + 8;

    for (int i = 0; i < 11; i++)
    {
        tft->setCursor(Config::MARGIN_X, startY + (i * 10));
        bool selected = (i == menuSelection);

        if (selected)
        {
            tft->setTextColor(COLOR_MENU_SELECTED);
            tft->setCursor(Config::MARGIN_X - 8, startY + (i * 10));
            tft->print(">");
        }
        else
        {
            tft->setTextColor(COLOR_MENU_NORMAL);
        }

        tft->setCursor(Config::MARGIN_X, startY + (i * 10));
        if (i < 10)
        {
            tft->printf("%d Laps", i + 1);
        }
        else
        {
            tft->print("Back");
        }
    }

    drawStatusBar(Config::SCREEN_HEIGHT - STATUS_BAR_HEIGHT);
}

void DisplayManager::drawDistanceSetMenu()
{
    static bool needsRedraw = true;
    static int lastSelection = -1;

    if (needsRedraw || lastSelection != menuSelection)
    {
        clearScreen();
        needsRedraw = false;
        lastSelection = menuSelection;
    }

    drawAnimatedHeader("DISTANCE SET", ST77XX_ORANGE);

    int startY = TITLE_HEIGHT + 8;

    for (int i = 0; i < 11; i++)
    {
        tft->setCursor(Config::MARGIN_X, startY + (i * 10));
        bool selected = (i == menuSelection);

        if (selected)
        {
            tft->setTextColor(COLOR_MENU_SELECTED);
            tft->setCursor(Config::MARGIN_X - 8, startY + (i * 10));
            tft->print(">");
        }
        else
        {
            tft->setTextColor(COLOR_MENU_NORMAL);
        }

        tft->setCursor(Config::MARGIN_X, startY + (i * 10));
        if (i < 10)
        {
            float distance = (i + 1) * 100.0f;
            tft->printf("%.0fm", distance);
        }
        else
        {
            tft->print("Back");
        }
    }

    drawStatusBar(Config::SCREEN_HEIGHT - STATUS_BAR_HEIGHT);
}

void DisplayManager::drawTimeSetMenu()
{
    static bool needsRedraw = true;
    static int lastSelection = -1;

    if (needsRedraw || lastSelection != menuSelection)
    {
        clearScreen();
        needsRedraw = false;
        lastSelection = menuSelection;
    }

    drawAnimatedHeader("TIME SET", ST77XX_YELLOW);

    int startY = TITLE_HEIGHT + 8;
    int visibleItems = 7;
    int scrollOffset = (menuSelection > 3) ? menuSelection - 3 : 0;

    for (int i = 0; i < visibleItems && (i + scrollOffset) < 34; i++)
    {
        int index = i + scrollOffset;
        tft->setCursor(Config::MARGIN_X, startY + (i * 10));
        bool selected = (index == menuSelection);

        if (selected)
        {
            tft->setTextColor(COLOR_MENU_SELECTED);
            tft->setCursor(Config::MARGIN_X - 8, startY + (i * 10));
            tft->print(">");
        }
        else
        {
            tft->setTextColor(COLOR_MENU_NORMAL);
        }

        tft->setCursor(Config::MARGIN_X, startY + (i * 10));
        if (index >= 0 && index < 34)
        {
            tft->print(timeOptions[index].displayText);
        }
    }

    if (scrollOffset > 0)
    {
        tft->setTextColor(ST77XX_CYAN);
        tft->setCursor(Config::SCREEN_WIDTH - 15, startY);
        tft->print("^");
    }
    if ((scrollOffset + visibleItems) < 34)
    {
        tft->setTextColor(ST77XX_CYAN);
        tft->setCursor(Config::SCREEN_WIDTH - 15, startY + (visibleItems - 1) * 10);
        tft->print("v");
    }

    drawStatusBar(Config::SCREEN_HEIGHT - STATUS_BAR_HEIGHT);
}

void DisplayManager::drawCenteredText(const String &text, int y, uint16_t color, int textSize)
{
    tft->setTextSize(textSize);
    tft->setTextColor(color);

    int textWidth = text.length() * 6 * textSize;
    int x = (Config::SCREEN_WIDTH - textWidth) / 2;

    if (x < Config::MARGIN_X)
    {
        x = Config::MARGIN_X;
    }

    tft->setCursor(x, y);
    tft->print(text);
}

void DisplayManager::drawMenuItem(const String &text, int index, int yPos, bool selected, bool animated)
{
    tft->setTextSize(1);
    tft->setCursor(Config::MARGIN_X, yPos);

    if (selected)
    {
        tft->setTextColor(COLOR_MENU_SELECTED);
        tft->setCursor(Config::MARGIN_X - 8, yPos);
        tft->print(">");
    }
    else
    {
        tft->setTextColor(COLOR_MENU_NORMAL);
    }

    tft->setCursor(Config::MARGIN_X, yPos);
    tft->print(text);
}

void DisplayManager::drawStatusBar(int y)
{
    RacingTelemetry &system = RacingTelemetry::getInstance();

    tft->drawLine(0, y - 1, Config::SCREEN_WIDTH, y - 1, COLOR_STATUS_BAR);

    tft->setTextSize(1);
    tft->setTextColor(COLOR_STATUS_BAR);
    tft->setCursor(Config::MARGIN_X, y);

    if (system.getStatus() == SystemStatus::RECORDING)
    {
        tft->println("Pin22:STOP Pin36:Nav Pin15:2s");
        tft->setCursor(Config::MARGIN_X, y + 10);
        tft->println("SELECT=Stop CURSOR=Nav");
    }
    else
    {
        tft->println("Pin22:SELECT Pin36:Nav Pin15:2s");
        tft->setCursor(Config::MARGIN_X, y + 10);
        tft->println("0.1s:Cursor 1s:Menu 2s:REC");
    }
}

void DisplayManager::drawTemperatureIndicator(float temp, int x, int y)
{
    tft->setTextSize(1);
    uint16_t tempColor = ST77XX_GREEN;

    if (temp >= 120.0f)
    {
        tempColor = ST77XX_RED;
    }
    else if (temp >= 80.0f)
    {
        tempColor = ST77XX_YELLOW;
    }

    tft->setTextColor(tempColor);
    tft->setCursor(x, y);
    tft->printf("Temp: %.1fC", temp);

    float tempPercent = (temp / 150.0f) * 100.0f;
    if (tempPercent > 100) tempPercent = 100;
    if (tempPercent < 0) tempPercent = 0;

    drawProgressBarAnimated(x, y + 10, 80, 4, tempPercent, tempColor);

    if (temp >= 120.0f && blinkState)
    {
        tft->setTextColor(ST77XX_RED);
        tft->setCursor(x + 85, y);
        tft->print("CRITICAL!");
    }
}

// **PERBAIKAN - GPS Status dengan clear area**
void DisplayManager::drawGPSStatus(int x, int y)
{
    SensorManager &sensors = SensorManager::getInstance();

    // **Clear GPS area first to prevent overlapping**
    tft->fillRect(x, y, Config::SCREEN_WIDTH - x - 5, 22, ST77XX_BLACK);

    tft->setTextSize(1);
    if (sensors.isGPSValid())
    {
        tft->setTextColor(COLOR_GPS_GOOD);
        tft->setCursor(x, y);
        tft->printf("GPS: %.4f,%.4f", sensors.getLatitude(), sensors.getLongitude());

        tft->setCursor(x, y + 11);
        tft->printf("Speed: %.1f km/h Sat: %d", sensors.getSpeed(), sensors.getSatelliteCount());

        if (animationFrame % 16 < 8)
        {
            tft->fillCircle(x + 120, y + 2, 2, COLOR_GPS_GOOD);
        }
    }
    else
    {
        tft->setTextColor(COLOR_GPS_BAD);
        tft->setCursor(x, y);
        tft->print("GPS: Searching...");

        tft->setCursor(x, y + 11);
        tft->print("No satellite fix");

        drawLoadingSpinner(x + 120, y - 5, COLOR_GPS_BAD);
    }
}


float DisplayManager::easeInOut(float t)
{
    return t * t * (3.0f - 2.0f * t);
}

uint16_t DisplayManager::interpolateColor(uint16_t color1, uint16_t color2, float ratio)
{
    if (ratio < 0.5f) return color1;
    return color2;
}

void DisplayManager::resetAnimations()
{
    lastAnimationUpdate = 0;
    animationFrame = 0;
    scrollOffset = 0.0f;
    blinkState = false;
    lastBlink = 0;
}

void DisplayManager::showStartupScreen()
{
    clearScreen();

    for (int phase = 0; phase < 3; phase++)
    {
        clearScreen();

        switch (phase)
        {
        case 0:
            drawCenteredText("ESP32 Racing", 30, ST77XX_GREEN);
            drawCenteredText("Telemetry", 45, ST77XX_GREEN);
            break;
        case 1:
            drawCenteredText("ESP32 Racing", 25, ST77XX_GREEN);
            drawCenteredText("Telemetry", 40, ST77XX_GREEN);
            drawCenteredText("2-Button System", 55, ST77XX_CYAN);
            break;
        case 2:
            drawCenteredText("ESP32 Racing", 20, ST77XX_GREEN);
            drawCenteredText("Telemetry", 35, ST77XX_GREEN);
            drawCenteredText("2-Button System", 50, ST77XX_CYAN);
            drawCenteredText("System Ready!", 80, ST77XX_WHITE);
            break;
        }
        delay(400);
    }

    delay(200);
}

void DisplayManager::enterMenu()
{
    inMenu = true;
    currentMenu = MenuState::MAIN;
    menuSelection = 0;
    resetAnimations();
}

void DisplayManager::exitMenu()
{
    inMenu = false;
    resetAnimations();
}

void DisplayManager::setConfiguration(DisplayConfiguration *cfg)
{
    config = cfg;
}



void DisplayManager::showSystemStatus()
{
    drawMainDisplay();
}
