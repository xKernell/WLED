//
// Created by Mohsen Basiti on 2024-11-10.
//
#pragma once
#ifdef USERMOD_CINEMAGIC_POWER

#include <INA226.h>
#include "cinemagic_shared.h"
#include "wled.h"    // Include WLED header to access strip and other variables

class CMPower {
public:
    CMShared *shared;

    explicit CMPower(CMShared *sh) : shared(sh) {  };

    INA226 *ina{nullptr};
    uint8_t mPwrIndex = 0;
    int32_t mVoltageBuffer[USERMODE_CINEMAGIC_MAX_READINGS] = {0};
    int32_t mCurrentBuffer[USERMODE_CINEMAGIC_MAX_READINGS] = {0};
    int64_t mVoltageSum{0};
    int64_t mCurrentSum{0};
    uint8_t initiated = 0;

    bool recalibrating = false;       // Flag for recalibration process

    // Variables to track previous values for change detection
    CMPowerMemory previousSavedData;
    unsigned long lastMemorySave{0};

    // Thresholds for detecting significant changes
    const float capacityChangeThreshold = 100.f; // e.g., 100 mAh
    const uint32_t cycleCountChangeThreshold = 1; // Save on every cycle increment


    void begin();

    void loop();

    static const char *getCapacityStateString(CapacityMeasurementState state);

    void updateDisplayString() const;

    void saveToFS(bool force = false);

    void loadFromFS() const;

    static void eraseMemory();

};

void CMPower::begin() {
//    eraseMemory();
    // Initialize buffers
    for (int i = 0; i < USERMODE_CINEMAGIC_MAX_READINGS; i++) {
        mVoltageBuffer[i] = 0;
        mCurrentBuffer[i] = 0;
    }

    // init ina226
    // Default INA226 address is 0x40
    ina = new INA226(Wire);
    if (!ina->begin(USERMOD_CINEMAGIC_INA_ADDR)) {
        DEBUG_PRINTLN("INA226 Connection error");
    }
    ina->configure(INA226_AVERAGES_1,
                   INA226_BUS_CONV_TIME_1100US,
                   INA226_SHUNT_CONV_TIME_1100US,
                   INA226_MODE_SHUNT_BUS_CONT);
    ina->calibrate(0.01, 8);

    // Load data from filesystem
    loadFromFS();

    // Initialize previous saved data
    previousSavedData = shared->power.memory;
}

void CMPower::loop() {
    uint32_t currentMillis = millis();
    if (millis() - shared->power.lastUpdate < USERMOD_CINEMAGIC_PWR_MEASURE_INTERVAL)
        return;
    uint32_t elapsedMillis = currentMillis - shared->power.lastUpdate;
    shared->power.lastUpdate = currentMillis;

    if (!ina) {
        DEBUG_PRINTLN("Cinemagic: Error in measuring power. INA is not initiated!");
        return;
    }

    // Read voltage and current using the library
    float voltage_V = ina->readBusVoltage(); // Voltage in V
    float current_A = ina->readShuntCurrent(); // Current in A

    // Convert to desired units
    int32_t newVoltage = (int32_t)(voltage_V * 100); // Convert V to 0.01V units
    int32_t newCurrent = (int32_t)(current_A * 100); // Convert A to 0.01A units

    // Update sums by subtracting the oldest reading and adding the new one
    mVoltageSum -= mVoltageBuffer[mPwrIndex];
    mCurrentSum -= mCurrentBuffer[mPwrIndex];

    mVoltageBuffer[mPwrIndex] = newVoltage;
    mCurrentBuffer[mPwrIndex] = newCurrent;

    mVoltageSum += newVoltage;
    mCurrentSum += newCurrent;

    // Compute averages once and store them
    shared->power.voltage = mVoltageSum / USERMODE_CINEMAGIC_MAX_READINGS;
    shared->power.current = mCurrentSum / USERMODE_CINEMAGIC_MAX_READINGS;

    // Circular buffer index
    mPwrIndex = (mPwrIndex + 1) % USERMODE_CINEMAGIC_MAX_READINGS;
    if (initiated < USERMODE_CINEMAGIC_MAX_READINGS + 5){
        initiated++;
        return;
    }

#ifdef USERMOD_CINEMAGIC_MAX_SAFE_CURRENT
    int16_t briMultiplierInt = briMultiplier;  // Retain as integer with one decimal place
    bool updateRequired = false;  // Track if update is needed

    if (shared->power.current > USERMOD_CINEMAGIC_MAX_SAFE_CURRENT *100 + 10) {
        briMultiplierInt = max((briMultiplierInt * 98) / 100, 10);
        updateRequired = true;
        shared->tempReduceCause = CURRENT;
    } else if (shared->power.current < USERMOD_CINEMAGIC_MAX_SAFE_CURRENT * 100 - 10 && shared->tempReduceCause == CURRENT) {
        briMultiplierInt = min((briMultiplierInt * 102) / 100, 100);
        updateRequired = true;
        if (briMultiplierInt == 100) {
            shared->tempReduceCause = NO_CAUSE;
        }
    }

    if (updateRequired) {
        briMultiplier = static_cast<byte>(briMultiplierInt);  // Convert back to integer percentage (0-100)
        strip.setBrightness((bri * briMultiplier) / 100);
        updateInterfaces(CALL_MODE_DIRECT_CHANGE);  // Ensure the UI updates
        strip.show();
        delay(150);
    }
#endif


#ifdef BATTERY_STATE_BASE_ON_VOLTAGE
    int delta = (shared->power.current * BATTERY_INTERNAL_R) / 10000;
    int estimatedOCV = shared->power.voltage + delta;

    // 3) Exponential smoothing on OCV for stability
    static int smoothedOCV = 0;
    if (smoothedOCV == 0) {
        // first call
        smoothedOCV = estimatedOCV;
    }
    // newSmoothed = (7*old + new) / 8  => alpha=1/8
    smoothedOCV = (7 * smoothedOCV + estimatedOCV) / 8;

    // 4) Clamp the smoothed voltage in [3.0..4.2] => [300..420]
    int clampMin = BATTERY_MIN_VOLTAGE, clampMax = BATTERY_FULL_VOLTAGE;
    int v = smoothedOCV;
    if (v < clampMin) v = clampMin;
    if (v > clampMax) v = clampMax;

    shared->power.batteryPercentage = (v - clampMin) * 100 / (clampMax - clampMin);
#else
    // Calculate consumed capacity (in mAh)
    int32_t current_mA = shared->power.current * 10;  // Current in mA
    float consumedCapacity = (float)current_mA * (float)elapsedMillis / TIME_SCALE;  // Result in mAh scaled

    // Update remaining capacity
    shared->power.memory.remainingCapacity -= consumedCapacity;

    // Ensure remainingCapacity stays within bounds
    if (shared->power.memory.remainingCapacity < 0) {
        shared->power.memory.remainingCapacity = 0;
    }
    if (shared->power.memory.remainingCapacity > shared->power.memory.totalCapacity) {
        shared->power.memory.remainingCapacity = shared->power.memory.totalCapacity;
    }

    // Handle capacity measurement
    switch (shared->power.memory.capacityState) {
     case WAITING_FOR_FULL:
        // Use hysteresis and time-based check
        static uint32_t fullVoltageTime = 0;
        if (shared->power.voltage >= BATTERY_FULL_VOLTAGE && shared->power.current > -10) {
            if (fullVoltageTime == 0) {
                fullVoltageTime = currentMillis;
            } else if (currentMillis - fullVoltageTime >= BATTERY_FULL_VOLTAGE_HOLD_TIME) {
                // Voltage has been above BATTERY_FULL_VOLTAGE for the required duration
                shared->power.memory.capacityState = MEASURING_CAPACITY;
                shared->power.memory.totalCapacity = 0;
                shared->power.memory.accumulatedConsumedCapacity = 0;
                fullVoltageTime = 0;
                DEBUG_PRINTLN("Battery fully charged. Starting capacity measurement.");
                saveToFS(true);  // Save immediately
            }
        } else {
            fullVoltageTime = 0;
        }
        break;

        case MEASURING_CAPACITY:
            // Accumulate capacity regardless of charging or discharging
            shared->power.memory.totalCapacity += consumedCapacity;
            shared->power.memory.accumulatedConsumedCapacity += consumedCapacity;

            // Check if battery changed to charging state
           if (shared->power.current < 0 && shared->power.voltage < 370) {
                // Capacity measurement complete
                shared->power.memory.capacityState = IDLE;
                shared->power.capacityInitialized = true;
                recalibrating = false;

                if (shared->power.memory.initialCapacity == 0) {
                    shared->power.memory.initialCapacity = shared->power.memory.totalCapacity;
                }

                shared->power.memory.remainingCapacity =
                    shared->power.memory.totalCapacity - shared->power.memory.accumulatedConsumedCapacity;

                DEBUG_PRINTLN("Battery capacity measurement complete.");
                DEBUG_PRINTF("Total capacity: %.2f mAh (scaled)\n", shared->power.memory.totalCapacity);
                saveToFS(true);  // Save immediately
            }
            break;

        case IDLE:
            // Check for recalibration trigger
            if ((shared->power.memory.cycleCount % CAPACITY_RECALIBRATION_CYCLES == 0 && shared->power.memory.cycleCount != 0 && shared->power.memory.initialCapacity != 0) ||
                    (shared->power.memory.cycleCount == 0 && shared->power.memory.totalCapacity == 0 && shared->power.memory.initialCapacity == 0)) {
                recalibrating = true;
                shared->power.memory.capacityState = WAITING_FOR_FULL;
            } else {
                if (current_mA >= 0) {
                    // Discharging
                    shared->power.memory.accumulatedConsumedCapacity += consumedCapacity;
                }

                // set battery is full
                if (shared->power.voltage >= BATTERY_FULL_VOLTAGE && current_mA >= -100 && current_mA <= 0){
                    shared->power.memory.remainingCapacity = shared->power.memory.totalCapacity;
                }

                // Check if accumulated consumed capacity exceeds total capacity
                if (shared->power.memory.accumulatedConsumedCapacity >= shared->power.memory.totalCapacity && shared->power.memory.totalCapacity > 0) {
                    // Increment the cycle count
                    shared->power.memory.cycleCount++;

                    // Subtract total capacity from accumulated consumed capacity
                    shared->power.memory.accumulatedConsumedCapacity -= shared->power.memory.totalCapacity;

                    // Ensure accumulatedConsumedCapacity doesn't go negative
                    if (shared->power.memory.accumulatedConsumedCapacity < 0) {
                        shared->power.memory.accumulatedConsumedCapacity = 0;
                    }

                    saveToFS(true);  // Save immediately to record the updated cycleCount
                }
            }
            break;

        default:
            break;
    }

    // Calculate battery percentage and health
    uint8_t batteryPercentage = 0;
    uint8_t batteryHealth = 0;

    if (shared->power.memory.totalCapacity > 0) {
        auto percentage = static_cast<uint8_t>((shared->power.memory.remainingCapacity * 100) / shared->power.memory.totalCapacity);
        percentage = constrain(percentage, 0, 100);
        batteryPercentage = percentage;
    }

    if (shared->power.memory.initialCapacity > 0) {
        auto health = static_cast<uint8_t>((shared->power.memory.totalCapacity * 100) / shared->power.memory.initialCapacity);
        health = constrain(health, 0, 100);
        batteryHealth = health;
    }

    // Update shared variables
    shared->power.batteryPercentage = batteryPercentage;
    shared->power.batteryHealth = batteryHealth;
#endif
    updateDisplayString();
    saveToFS();
}

const char *CMPower::getCapacityStateString(CapacityMeasurementState state) {
    switch (state) {
        case IDLE:
            return "IDLE";
        case WAITING_FOR_FULL:
            return "WAITING";
        case MEASURING_CAPACITY:
            return "MEASURING";
        default:
            return "UNKNOWN";
    }
}

void CMPower::updateDisplayString() const {
    int32_t currentValue = shared->power.current;
    char sign = (currentValue < 0) ? '-' : '+';
    currentValue = abs(currentValue);

    snprintf(shared->power.displayStr, sizeof(shared->power.displayStr),
             "V:%d.%02dV I:%c%d.%02dA B:%u%% H:%u%% FC:%.2fmAh CC:%.2fmAh State:%s CYC:%d",
             shared->power.voltage / 100, abs(shared->power.voltage % 100),
             sign, currentValue / 100, currentValue % 100,
             shared->power.batteryPercentage,
             shared->power.batteryHealth,
             shared->power.memory.totalCapacity,
             shared->power.memory.remainingCapacity,
             getCapacityStateString(shared->power.memory.capacityState),
             shared->power.memory.cycleCount);
}

void CMPower::loadFromFS() const {
    // Open file for reading
    File file = WLED_FS.open("/powerData.dat", "r");
    if (!file) {
        shared->power.capacityInitialized = false;
        shared->power.memory.capacityState = WAITING_FOR_FULL;
        DEBUG_PRINTLN("No power data file found, using default values");
        return;
    }

    // Read data into memory struct
    size_t readBytes = file.read((uint8_t *)&shared->power.memory, sizeof(CMPowerMemory));
    file.close();

    if (readBytes != sizeof(CMPowerMemory)) {
        DEBUG_PRINTLN("Failed to read complete data from file");
        shared->power.capacityInitialized = false;
        shared->power.memory.capacityState = WAITING_FOR_FULL;
        return;
    }

    shared->power.capacityInitialized = (shared->power.memory.totalCapacity > 0);

    // Log the loaded data for debugging
    DEBUG_PRINTLN("Power data loaded from LittleFS");
    DEBUG_PRINTF("Saved capacityState: %d\n", shared->power.memory.capacityState);
    DEBUG_PRINTF("Saved totalCapacity: %.2f\n", shared->power.memory.totalCapacity);
    DEBUG_PRINTF("Saved remainingCapacity: %.2f\n", shared->power.memory.remainingCapacity);
    DEBUG_PRINTF("Saved initialCapacity: %.2f\n", shared->power.memory.initialCapacity);
    DEBUG_PRINTF("Saved cycleCount: %u\n", shared->power.memory.cycleCount);
}

void CMPower::saveToFS(bool force) {
    if (strip.isUpdating() && shared->power.memory.capacityState == previousSavedData.capacityState && !force) {
        // Defer saving until strip is not updating
        return;
    }

    uint32_t currentMillis = millis();

    // Check if significant changes have occurred
    bool capacityChanged = abs(shared->power.memory.remainingCapacity - previousSavedData.remainingCapacity) >= capacityChangeThreshold;
    bool totalCapacityChanged = abs(shared->power.memory.totalCapacity - previousSavedData.totalCapacity) >= capacityChangeThreshold;
    bool cycleCountChanged = (shared->power.memory.cycleCount - previousSavedData.cycleCount) >= cycleCountChangeThreshold;
    bool statusChanged = (shared->power.memory.capacityState != previousSavedData.capacityState);

    // If statusChanged is true, bypass the interval check
    if ((currentMillis - lastMemorySave < USERMOD_CINEMAGIC_PWR_MEMORY_UPDATE_INTERVAL) && !statusChanged) {
        return;
    }

    // If no significant changes, no need to save
    if (!(capacityChanged || totalCapacityChanged || cycleCountChanged || statusChanged)) {
        return;
    }

    // Open file for writing
    File file = WLED_FS.open("/powerData.dat", "w");
    if (!file) {
        DEBUG_PRINTLN("Failed to open file for writing");
        return;
    }

    // Write memory struct to file
    size_t written = file.write((uint8_t *)&shared->power.memory, sizeof(CMPowerMemory));
    if (written != sizeof(CMPowerMemory)) {
        DEBUG_PRINTLN("Failed to write complete data to file");
        file.close();
        return;
    }

    // Flush the file buffer to ensure data is written to flash
    file.flush();
    file.close();

    lastMemorySave = currentMillis;
    previousSavedData = shared->power.memory;
    DEBUG_PRINTLN("Power data saved to LittleFS");
    DEBUG_PRINTF("Saved capacityState: %d\n", shared->power.memory.capacityState);
    DEBUG_PRINTF("Saved totalCapacity: %.2f\n", shared->power.memory.totalCapacity);
    DEBUG_PRINTF("Saved remainingCapacity: %.2f\n", shared->power.memory.remainingCapacity);
    DEBUG_PRINTF("Saved initialCapacity: %.2f\n", shared->power.memory.initialCapacity);
    DEBUG_PRINTF("Saved cycleCount: %u\n", shared->power.memory.cycleCount);

    // Add a small delay to ensure the filesystem completes the write
    delay(1);  // Adjust the delay as necessary
}

void CMPower::eraseMemory(){
    // Open file for writing
    File file = WLED_FS.open("/powerData.dat", "w");
    if (!file) {
        DEBUG_PRINTLN("Failed to open file for writing");
        return;
    }

    CMPowerMemory emptyMemory;

    // Write memory struct to file
    size_t written = file.write((uint8_t *)&emptyMemory, sizeof(CMPowerMemory));
    if (written != sizeof(CMPowerMemory)) {
        DEBUG_PRINTLN("Failed to write complete data to file");
        file.close();
        return;
    }

    // Flush the file buffer to ensure data is written to flash
    file.flush();
    file.close();
    delay(1000);
}
#endif
