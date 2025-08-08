#include "SensorManager.h"

SensorManager::SensorManager() 
    : gps(nullptr), gpsSerial(nullptr), tempSensor(nullptr), oneWire(nullptr),
      lastSensorUpdate(0), lastGPSUpdate(0) {
}

SensorManager::~SensorManager() {
    delete gps;
    delete gpsSerial;
    delete tempSensor;
    delete oneWire;
}
// Static instance pointer
SensorManager* SensorManager::instance = nullptr;

// Interrupt handler function
void IRAM_ATTR SensorManager::rpmInterruptHandler() {
    if (instance) {
        instance->rpmPulseCount++;
    }
}

// RPM sensor reading with interrupt
float SensorManager::readRPMSensor() {
    unsigned long currentTime = millis();
    unsigned long timeInterval = currentTime - lastRPMCalculation;
    
    // Hitung RPM setiap 1000ms
    if (timeInterval >= 1000) {
        // Disable interrupt sementara untuk membaca data
        noInterrupts();
        unsigned long pulses = rpmPulseCount;
        rpmPulseCount = 0;
        interrupts();
        
        // Hitung RPM
        // Untuk sensor yang memberikan 1 pulse per revolution
        currentRPM = (pulses * 60.0) / (timeInterval / 1000.0);
        
        // Konstrain dan filter
        currentRPM = constrain(currentRPM, 0, 8000);
        if (currentRPM < 200) currentRPM = 0;
        
        lastRPMCalculation = currentTime;
    }
    
    return currentRPM;
}

void SensorManager::initialize() {
    // Initialize analog sensor pins
    pinMode(Config::PIN_AFR, INPUT);
    pinMode(Config::PIN_MAP, INPUT);
    instance = this;
    rpmPulseCount = 0;
    lastRPMCalculation = millis();
    currentRPM = 0.0;
    
    pinMode(Config::PIN_RPM, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(Config::PIN_RPM), rpmInterruptHandler, FALLING);
    pinMode(Config::PIN_TPS, INPUT);
    pinMode(Config::PIN_INCLINE, INPUT);
    pinMode(Config::PIN_STROKE, INPUT);

    
    // Initialize GPS
    gps = new TinyGPSPlus();
    gpsSerial = new HardwareSerial(2);
    gpsSerial->begin(9600, SERIAL_8N1, Config::GPS_RX, Config::GPS_TX);
    
    // Initialize temperature sensor
    oneWire = new OneWire(Config::PIN_TEMP);
    tempSensor = new DallasTemperature(oneWire);
    tempSensor->begin();
    
    Serial.println("=== Sensors Initialized ===");
    Serial.printf("AFR Sensor: Pin %d\n", Config::PIN_AFR);
    Serial.printf("MAP Sensor: Pin %d\n", Config::PIN_MAP);
    Serial.printf("TPS Sensor: Pin %d\n", Config::PIN_TPS);
    Serial.printf("Incline Sensor: Pin %d\n", Config::PIN_INCLINE);
    Serial.printf("Stroke Sensor: Pin %d\n", Config::PIN_STROKE);
    Serial.printf("Temperature Sensor: Pin %d (DS18B20)\n", Config::PIN_TEMP);
    Serial.printf("GPS: RX=%d, TX=%d\n", Config::GPS_RX, Config::GPS_TX);
}

void SensorManager::update() {
    unsigned long currentTime = millis();
    
    if (currentTime - lastSensorUpdate >= Config::SENSOR_UPDATE_INTERVAL) {
        // Read all sensors
        // currentData.afr = readAFRSensor(); // real pembacaan
        currentData.afr = random(11.20, 12.00); // For testing  ();
        currentData.rpm = readRPMSensor();
        currentData.temp = readTemperatureSensor();
        currentData.tps = readTPSSensor();
        currentData.map_value = readMAPSensor();
        currentData.incline = 0.0;
        currentData.stroke = 0.0;
        currentData.timestamp = currentTime;
        
        // Update GPS data if available
        if (gps->location.isValid()) {
            currentData.lat = gps->location.lat();
            currentData.lng = gps->location.lng();
            currentData.speed = gps->speed.kmph();
        } else {
            currentData.speed = 0.0;
        }
        
        lastSensorUpdate = currentTime;
    }
}

void SensorManager::updateGPS() {
    unsigned long currentTime = millis();
    
    if (currentTime - lastGPSUpdate >= Config::GPS_UPDATE_INTERVAL) {
        while (gpsSerial->available()) {
            gps->encode(gpsSerial->read());
        }
        lastGPSUpdate = currentTime;
    }
}

float SensorManager::readAFRSensor() {
    int rawValue = analogRead(Config::PIN_AFR);
    float voltage = rawValue * (5.0 / 4095.0);
    float afr = 10.0 + (voltage * 2.0);
    
    if (afr < 10.0) afr = 10.0;
    if (afr > 20.0) afr = 20.0;
    
    return afr;
}

float SensorManager::readMAPSensor() {
    int rawValue = analogRead(Config::PIN_MAP);
    float voltage = rawValue * (5.0 / 4095.0);
    float pressure = ((voltage - 0.5) / 4.0) * 250.0;
    
    if (pressure < 0) pressure = 0;
    if (pressure > 250) pressure = 250;
    
    return pressure;
}

// float SensorManager::readTPSSensor() { // ini yang belum di pulldown
//     // Baca nilai ADC mentah
//     int rawValue = analogRead(Config::PIN_TPS);
    
//     // Konversi ke persentase (0-100% berdasarkan ADC 12-bit)
//     float percentage = (rawValue / 4095.0) * 100.0;
    
//     // Konstrain nilai dalam range yang valid
//     percentage = constrain(percentage, 0, 100);
    
//     // Mapping dari range aktual (16-100) ke range yang diinginkan (0-100)
//     if (percentage >= 16.0) {
//         // Gunakan map() dengan floating point
//         percentage = ((percentage - 16.0) / (100.0 - 16.0)) * 100.0;
//     } else {
//         // Jika pembacaan di bawah 16%, anggap sebagai 0%
//         percentage = 0.0;
//     }
    
//     // Konstrain hasil akhir
//     percentage = constrain(percentage, 0, 100);
    
//     return percentage;
// }
float SensorManager::readTPSSensor() {
    // Variabel untuk smoothing dan filtering
    static float previousReading = 0.0;
    static unsigned long lastReadTime = 0;
    static float smoothedValue = 0.0;
    
    // Multi-sampling untuk mengurangi noise
    const int numSamples = 10;
    unsigned long totalReading = 0;
    
    // Ambil multiple samples untuk averaging
    for (int i = 0; i < numSamples; i++) {
        totalReading += analogRead(Config::PIN_TPS);
        delayMicroseconds(100); // Small delay between readings
    }
    
    // Hitung rata-rata ADC value
    float avgRawValue = totalReading / (float)numSamples;
    
    // Konversi ke voltage (ESP32 ADC 12-bit, 3.3V reference)
    float voltage = avgRawValue * (5.0 / 4095.0);
    
    // Kalibrasi berdasarkan karakteristik TPS
    // TPS standar: Idle = 0.6-0.9V, WOT = 3.5-4.7V
    // Mapping voltage ke percentage
    float percentage;
    
    if (voltage <= 1.2) {
        // Throttle tertutup penuh
        percentage = 0.0;
    } else {
        // Linear mapping dari voltage ke percentage
        percentage = ((voltage - 0.6) / (4.5 - 0.6)) * 100.0;
    }
    
    // Konstrain hasil dalam range 0-100%
    percentage = constrain(percentage, 0.0, 100.0);
    
    // Terapkan low-pass filter untuk smoothing
    unsigned long currentTime = millis();
    if (currentTime - lastReadTime > 20) { // Update setiap 50ms
        float alpha = 0.2; // Smoothing factor (0-1, semakin kecil semakin smooth)
        smoothedValue = (alpha * percentage) + ((1 - alpha) * smoothedValue);
        lastReadTime = currentTime;
    }
    
    // Dead zone untuk mengurangi jitter di idle
    if (smoothedValue < 2.0) {
        smoothedValue = 0.0;
    }
    
    return smoothedValue;
}


float SensorManager::readInclineSensor() {
    int rawValue = analogRead(Config::PIN_INCLINE);
    float angle = ((rawValue / 4095.0) * 90.0) - 45.0;
    
    if (angle < -45.0) angle = -45.0;
    if (angle > 45.0) angle = 45.0;
    
    return angle;
}

float SensorManager::readStrokeSensor() {
    int rawValue = analogRead(Config::PIN_STROKE);
    float stroke = (rawValue / 4095.0) * 50.0;
    
    if (stroke < 0) stroke = 0;
    if (stroke > 50) stroke = 50;
    
    return stroke;
}

float SensorManager::readTemperatureSensor() {
    tempSensor->requestTemperatures();
    float temp = tempSensor->getTempCByIndex(0);
    
    if (isnan(temp) || temp == DEVICE_DISCONNECTED_C) {
        Serial.println("Warning: Temperature sensor error, using default value");
        temp = 85.0;
    }
    
    if (temp < -40.0) temp = -40.0;
    if (temp > 150.0) temp = 150.0;
    return temp;
}

float SensorManager::estimateRPM() {
    float baseRPM = 800.0;
    float tpsContribution = currentData.tps * 60.0;
    float mapContribution = currentData.map_value * 5.0;
    
    float estimatedRPM = baseRPM + tpsContribution + mapContribution;
    estimatedRPM += random(-50, 50);
    
    if (estimatedRPM < 500) estimatedRPM = 500;
    if (estimatedRPM > 8000) estimatedRPM = 8000;
    
    return estimatedRPM;
}


bool SensorManager::isGPSValid() const {
    return gps->location.isValid();
}

double SensorManager::getLatitude() const {
    return gps->location.lat();
}

double SensorManager::getLongitude() const {
    return gps->location.lng();
}

float SensorManager::getSpeed() const {
    return gps->speed.kmph();
}

int SensorManager::getSatelliteCount() const {
    return gps->satellites.value();
}

void SensorManager::logSensorData() {
    static unsigned long lastLog = 0;
    if (millis() - lastLog >= 5000) {
        Serial.printf("SENSORS - AFR:%.1f RPM:%.0f TEMP:%.1f TPS:%.1f MAP:%.1f GPS:%s\n",
                      currentData.afr, currentData.rpm, currentData.temp, 
                      currentData.tps, currentData.map_value,
                      isGPSValid() ? "OK" : "NO_FIX");
        lastLog = millis();
    }
}
