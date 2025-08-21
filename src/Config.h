#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <TinyGPS++.h>
#include <HardwareSerial.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPIFFS.h>
#include <math.h>
// Config.h
extern bool realTime;


// === COLOR DEFINITIONS ===
#ifndef ST77XX_BLACK
#define ST77XX_BLACK 0x0000
#define ST77XX_BLUE 0x001F
#define ST77XX_RED 0xF800
#define ST77XX_GREEN 0x07E0
#define ST77XX_CYAN 0x07FF
#define ST77XX_MAGENTA 0xF81F
#define ST77XX_YELLOW 0xFFE0
#define ST77XX_WHITE 0xFFFF
#define ST77XX_GRAY 0x8410
#endif

class Config
{
public:
  // System Constants
  static const int TRAIN_DATA_SIZE = 200; // Updated to 200 samples
  static const int K_VALUE = 3;
  static const int NUM_CLASSES = 4; // Updated to 4 classes

  // Classification thresholds for hybrid approach
  const float COLD_ENGINE_THRESHOLD = 65.0; // °C
  const float WARM_ENGINE_THRESHOLD = 75.0; // °C
  const float CRITICAL_AFR_MIN = 11.0;
  const float CRITICAL_AFR_MAX = 12.0;
  static const int TIME_OPTIONS_COUNT = 34;

  // Display Constants
  static const int SCREEN_WIDTH = 160;
  static const int SCREEN_HEIGHT = 200;
  static const int HEADER_HEIGHT = 12;
  static const int LINE_HEIGHT = 10;
  static const int MARGIN_X = 6;
  static const int MARGIN_Y = 12;

  // Pin Definitions
  static const int TFT_CS = 5;
  static const int TFT_RST = 4;
  static const int TFT_DC = 2;
  static const int TFT_SCLK = 18;
  static const int TFT_MOSI = 23;

  static const int GPS_RX = 16;
  static const int GPS_TX = 17;

  static const int PIN_AFR = 26;
  static const int PIN_MAP = 35;
  static const int PIN_TPS = 32;
  static const int PIN_INCLINE = 15;
  static const int PIN_RPM = 25;

  static const int PIN_STROKE = 99;
  static const int PIN_TEMP = 33;

  static const int PIN_EWP = 19;
  static const int PIN_FAN = 14;
  static const int PIN_CUTOFF = 12;

  static const int BTN_CURSOR = 36;
  static const int BTN_TX = 39;
  static const int LED_PIN = 34;
  // Timing Constants
  // Timing Constants - UPDATED
  static const int BTN_REC = 21;                      // Pin 15 untuk RECORDING (khusus)
  static const unsigned long CURSOR_PRESS_TIME = 10;  // 0.1s untuk cursor/navigation
  static const unsigned long MENU_PRESS_TIME = 100;   // 1s untuk masuk menu
  static const unsigned long RECORD_PRESS_TIME = 100; // 3s untuk recording
  static const unsigned long SHORT_PRESS_TIME = 10;   // Minimum press detection
  static const unsigned long SENSOR_UPDATE_INTERVAL = 100;
  static const unsigned long GPS_UPDATE_INTERVAL = 300;
  static const unsigned long COOLING_UPDATE_INTERVAL = 100;
  static const unsigned long CLASSIFICATION_INTERVAL = 100;
  static const unsigned long HEALTH_CHECK_INTERVAL = 100;
  static const unsigned long RESPONSIVE_PRESS_TIME = 10; // 0.1s untuk RESPONSIVE_PRESS_TIME

  // Temperature Settings
  static constexpr float DEFAULT_FAN_TEMP = 80.0f;
  static constexpr float DEFAULT_CUTOFF_TEMP = 120.0f;
  static constexpr float TEMP_HYSTERESIS_FAN = 2.0f;
  static constexpr float TEMP_HYSTERESIS_CUTOFF = 5.0f;

  // System Settings
  static const int MIN_FREE_HEAP = 10000;
  static const int DEFAULT_REFRESH_RATE = 300;
};

#endif // CONFIG_H
