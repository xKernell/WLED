//
// Created by Mohsen Basiti on 2024-11-11.
//

#ifndef WLED_CINEMAGIC_SHARED_H
#define WLED_CINEMAGIC_SHARED_H

#include "Arduino.h"

static const uint16_t WARMEST_KELVIN = 1900;
static const uint16_t COLDEST_KELVIN = 6000;

// View management
enum DisplayView {
    NO_VIEW = 0,
    STARTUP_VIEW,
    MAIN_VIEW,
    BATTERY_VIEW,
    TEMPERATURE_VIEW,
    NETWORK_VIEW,
    SETTING_VIEW,
    NODE_VIEWS
};
enum DisplayItem {
    NO_ITEM = 0,
    BRIGHTNESS,
    COLOR_TEMPERATURE,
    HSI,
    EFFECT,
    PALETTE,
    EFFECT_SPEED,
    EFFECT_INTENSITY
};
enum DisplayMode {
    CCT_MODE = 0,
    HSI_MODE,
    EFFECT_MODE,
    PRESET_MODE
};
#define MAX_MODE_ITEMS 6
static const DisplayItem DisplayModeItems[4][MAX_MODE_ITEMS] = {
        // CCT_MODE
        { BRIGHTNESS, COLOR_TEMPERATURE, NO_ITEM, NO_ITEM, NO_ITEM, NO_ITEM },

        // HSI_MODE
        { BRIGHTNESS, HSI, NO_ITEM, NO_ITEM, NO_ITEM, NO_ITEM },

        // EFFECT_MODE
        { BRIGHTNESS, EFFECT, EFFECT_SPEED, PALETTE, NO_ITEM, NO_ITEM },

        // PRESET_MODE
        { BRIGHTNESS, NO_ITEM, NO_ITEM, NO_ITEM, NO_ITEM, NO_ITEM }
};

enum BrightnessDecreaseCause : uint8_t {
    NO_CAUSE,
    CURRENT,
    TEMPERATURE,
    CRITICAL_TEMPERATURE
};

#ifdef USERMOD_CINEMAGIC_POWER
#ifndef USERMODE_CINEMAGIC_MAX_READINGS
#define USERMODE_CINEMAGIC_MAX_READINGS 5
#endif

#ifndef USERMOD_CINEMAGIC_PWR_MEASURE_INTERVAL
#define USERMOD_CINEMAGIC_PWR_MEASURE_INTERVAL 300
#endif

#ifndef USERMOD_CINEMAGIC_PWR_MEMORY_UPDATE_INTERVAL
#define USERMOD_CINEMAGIC_PWR_MEMORY_UPDATE_INTERVAL 60000 // Interval to write to memory (e.g., every 60 seconds)
#endif

#ifndef USERMOD_CINEMAGIC_BATT_CAPACITY
#define USERMOD_CINEMAGIC_BATT_CAPACITY 3000 // mA
#endif

#ifndef USERMOD_CINEMAGIC_BATT_CELLS
#define USERMOD_CINEMAGIC_BATT_CELLS 1
#endif

#ifndef USERMOD_CINEMAGIC_BATT_MAX_VOLTAGE
#define USERMOD_CINEMAGIC_BATT_MAX_VOLTAGE 4.19
#endif

#ifndef USERMOD_CINEMAGIC_BATT_MIN_VOLTAGE
#define USERMOD_CINEMAGIC_BATT_MIN_VOLTAGE 3.0
#endif

#ifndef USERMOD_CINEMAGIC_INA_ADDR
#define USERMOD_CINEMAGIC_INA_ADDR 0x40
#endif

#ifndef BATTERY_FULL_VOLTAGE
#define BATTERY_FULL_VOLTAGE 412          // Full battery voltage in volts
#endif

#ifndef BATTERY_MIN_VOLTAGE
#define BATTERY_MIN_VOLTAGE 305            // Minimum battery voltage in volts
#endif

#ifndef BATTERY_INTERNAL_R
#define BATTERY_INTERNAL_R 30            // mOhms
#endif

#ifndef CAPACITY_RECALIBRATION_CYCLES
#define CAPACITY_RECALIBRATION_CYCLES 20    // Number of cycles before recalibration
#endif

#define BATTERY_STATE_BASE_ON_VOLTAGE

// Define scaling factors
#ifndef VOLTAGE_SCALE
#define VOLTAGE_SCALE 100       // Voltage in 0.01V units
#endif

#ifndef CURRENT_SCALE
#define CURRENT_SCALE 100       // Current in 0.01A units
#endif

#ifndef TIME_SCALE
#define TIME_SCALE 3600000      // Time scaling factor for converting ms to hours (ms in an hour)
#endif

#ifndef BATTERY_FULL_VOLTAGE_HOLD_TIME
#define BATTERY_FULL_VOLTAGE_HOLD_TIME 60*1000 // 2.5min
#endif

enum CapacityMeasurementState : uint8_t {
    IDLE,
    WAITING_FOR_FULL,
    MEASURING_CAPACITY
};

#pragma pack(push, 1)
struct CMPowerMemory {
    float totalCapacity{0};
    float remainingCapacity{0};
    uint32_t cycleCount{0};
    float initialCapacity{0};
    float accumulatedConsumedCapacity = 0; // Accumulated consumed capacity in mAh scaled
    CapacityMeasurementState capacityState{IDLE};
};
#pragma pack(pop)

struct CMPowerModel {
    unsigned long lastUpdate{0};

    int32_t voltage{0};
    int32_t current{0};
    unsigned long cycle{0};

    // Battery capacity measurement variables (all scaled)
    uint8_t batteryPercentage = 0;
    uint8_t batteryHealth = 0;
    CMPowerMemory memory;
    bool capacityInitialized = false; // Flag to check if capacity has been initialized

    char displayStr[128];
};

#endif

#ifdef USERMOD_CINEMAGIC_TEMPERATURE
#ifndef USERMOD_CINEMAGIC_TEMP_MEASURE_INTERVAL
#define USERMOD_CINEMAGIC_TEMP_MEASURE_INTERVAL 300
#endif

struct CMTemperatureModel {
    int16_t max{0};
    int16_t cpu{0};
    int16_t led{0};
    int16_t board{0};
    unsigned long lastUpdate{0};
};
#endif

struct CMShared {
#ifdef USERMOD_CINEMAGIC_POWER
    CMPowerModel power;
#endif
#ifdef USERMOD_CINEMAGIC_TEMPERATURE
    CMTemperatureModel temp;
#endif
    bool apMode = false;
    String ssid = apSSID;
    IPAddress ip = IPAddress(4, 3, 2, 1);
    BrightnessDecreaseCause tempReduceCause{NO_CAUSE};

    DisplayView currentView = STARTUP_VIEW;
    DisplayItem currentItem = BRIGHTNESS;
    DisplayMode currentMode = CCT_MODE;

    uint8_t ledCCTTemp = 20;
    uint16_t ledHue = 150;

    const char *getTempReduceCauseString() {
        switch (tempReduceCause) {
            case NO_CAUSE:
                return "NONE";
            case CURRENT:
                return "CURRENT";
            case TEMPERATURE:
                return "TEMPERATURE";
            case CRITICAL_TEMPERATURE:
                return "CRITICAL_TEMPERATURE";
            default:
                return "UNKNOWN";
        }
    }

};


#ifdef USERMOD_CINEMAGIC_OLED
#define SCREEN_ADDRESS  0x3C

//#define LCD_ON_SEPRATE_THREAD
#define SCREEN_TIMEOUT_MS  60*1000    // When to time out to the clock or blank the screen, if SLEEP_MODE_ENABLED.
#define REFRESH_RATE_MS 100 // Minimum time between redrawing screen in ms
#define LINE_BUFFER_SIZE            16+1
#define MAX_JSON_CHARS              19+1
#define MAX_MODE_LINE_SPACE         7+1

#if CONFIG_FREERTOS_UNICORE
//#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

typedef enum {
    NONE = 0,
    SSD1306,          // U8X8_SSD1306_128X32_UNIVISION_HW_I2C
    SH1106,           // U8X8_SH1106_128X64_WINSTAR_HW_I2C
    SSD1306_64,       // U8X8_SSD1306_128X64_NONAME_HW_I2C
    SSD1305,          // U8X8_SSD1305_128X32_ADAFRUIT_HW_I2C
    SSD1305_64,       // U8X8_SSD1305_128X64_ADAFRUIT_HW_I2C
    SSD1306_SPI,      // U8X8_SSD1306_128X32_NONAME_HW_SPI
    SSD1306_SPI64,    // U8X8_SSD1306_128X64_NONAME_HW_SPI
    SSD1309_SPI64     // U8X8_SSD1309_128X64_NONAME0_4W_HW_SPI
} DisplayType;

#endif

#endif //WLED_CINEMAGIC_SHARED_H
