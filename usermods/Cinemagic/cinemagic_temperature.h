//
// Created by Mohsen Basiti on 2024-11-11.
//

#pragma once
#ifdef USERMOD_CINEMAGIC_TEMPERATURE
#include "cinemagic_shared.h"

extern "C" uint8_t temprature_sens_read();

#ifndef USERMOD_CINEMAGIC_TEMP_LEDPANEL_PIN
#define USERMOD_CINEMAGIC_TEMP_LEDPANEL_PIN -1
#endif

#ifndef USERMOD_CINEMAGIC_TEMP_BOARD_PIN
#define USERMOD_CINEMAGIC_TEMP_BOARD_PIN -1
#endif


class CMTemperature {
public:
    CMShared *shared;

    CMTemperature(CMShared *sh) : shared(sh) {};
    unsigned long lastTempBriAdjust{0};
    uint8_t mCriticalOrigTemp{false};

    void begin();
    void loop();
    void updateBrightnessCap(const int16_t bright){
        briMultiplier = static_cast<byte>(bright);  // Convert back to integer percentage (0-100)
        strip.setBrightness((bri * briMultiplier) / 100);
        updateInterfaces(CALL_MODE_DIRECT_CHANGE);  // Ensure the UI updates
        strip.show();
        delay(150);
    }
};

void CMTemperature::begin() {
    analogReadResolution(12); // Set the resolution to 12-bit
    if (USERMOD_CINEMAGIC_TEMP_LEDPANEL_PIN >= 0 && PinManager::allocatePin(USERMOD_CINEMAGIC_TEMP_LEDPANEL_PIN, true, PinOwner::UM_CINEMAGIC)) {
        pinMode(USERMOD_CINEMAGIC_TEMP_LEDPANEL_PIN, INPUT);
    }
    if (USERMOD_CINEMAGIC_TEMP_BOARD_PIN >= 0 && PinManager::allocatePin(USERMOD_CINEMAGIC_TEMP_BOARD_PIN, true, PinOwner::UM_CINEMAGIC)) {
        pinMode(USERMOD_CINEMAGIC_TEMP_BOARD_PIN, INPUT);
    }
}

void CMTemperature::loop() {
    if (millis() - shared->temp.lastUpdate > USERMOD_CINEMAGIC_TEMP_MEASURE_INTERVAL) {
        shared->temp.lastUpdate = millis();

        const uint32_t R25 = 10000;        // Resistance at 25°C (10k ohms)
        const uint32_t B = 3950;           // B constant
        const uint32_t ADC_MAX = 4095;     // 12-bit ADC maximum value

        uint16_t analogValue;
        float resistance, tempK, temperatureC;
        int16_t temperatureC_x100; // Temperature in 0.01°C units

#ifdef ARDUINO_ARCH_ESP32
        // CPU Temperature in 0.01°C units
        shared->temp.cpu = static_cast<int16_t>(((temprature_sens_read() - 32) * 1000) / 18);
#endif

        if (USERMOD_CINEMAGIC_TEMP_LEDPANEL_PIN > -1) {
            analogValue = analogRead(USERMOD_CINEMAGIC_TEMP_LEDPANEL_PIN);
            if (analogValue == 0) analogValue = 1; // Prevent division by zero
            resistance = 10000.0f * (ADC_MAX / analogValue - 1.0f);
            tempK = 1.0f / ((logf(resistance / R25) / B) + (1.0f / 298.15f));
            temperatureC = tempK - 273.15f;
            temperatureC_x100 = static_cast<int16_t>(temperatureC * 100.0f);
            shared->temp.led = -temperatureC_x100 + 4700; // mTemp1 in 0.01°C units
            if (shared->temp.led > 20000){
                shared->temp.led = 0;
            }
        }

        if (USERMOD_CINEMAGIC_TEMP_BOARD_PIN > -1) {
            analogValue = analogRead(USERMOD_CINEMAGIC_TEMP_BOARD_PIN);
            if (analogValue == 0) analogValue = 1; // Prevent division by zero
            resistance = 10000.0f * (ADC_MAX / analogValue - 1.0f);
            tempK = 1.0f / ((logf(resistance / R25) / B) + (1.0f / 298.15f));
            temperatureC = tempK - 273.15f;
            temperatureC_x100 = static_cast<int16_t>(temperatureC * 100.0f);
            shared->temp.board = -temperatureC_x100 + 4700; // mTemp2 in 0.01°C units
            if (shared->temp.board > 20000){
                shared->temp.board = 0;
            }
        }
        shared->temp.max = max(shared->temp.led, shared->temp.board);

#ifdef USERMOD_CINEMAGIC_CRITICAL_TEMP
        if (shared->temp.board > USERMOD_CINEMAGIC_CRITICAL_TEMP * 100){
            mCriticalOrigTemp = briMultiplier;
            shared->tempReduceCause = CRITICAL_TEMPERATURE;
            updateBrightnessCap(1);
        } else if (shared->tempReduceCause == CRITICAL_TEMPERATURE && shared->temp.board < USERMOD_CINEMAGIC_CRITICAL_TEMP * 100 - 500){
            shared->tempReduceCause = NO_CAUSE;
            updateBrightnessCap(briMultiplier);
        }
#endif
#ifdef USERMOD_CINEMAGIC_MAX_SAFE_TEMP
        if (millis() - lastTempBriAdjust > 60000 && shared->tempReduceCause != CRITICAL_TEMPERATURE){
            static int16_t briMultiplierInt = briMultiplier;  // Retain as integer with one decimal place
            bool updateRequired = false;  // Track if update is needed


            if (shared->temp.max > USERMOD_CINEMAGIC_MAX_SAFE_TEMP * 100 + 100) {
                briMultiplierInt = max((briMultiplierInt * 95) / 100, 10);  // Ensure it doesn't go below 10.0 (represented as 100)
                updateRequired = true;
                shared->tempReduceCause = TEMPERATURE;
            } else if (shared->temp.max < USERMOD_CINEMAGIC_MAX_SAFE_TEMP * 100 - 100 && briMultiplierInt < 100 && shared->tempReduceCause == TEMPERATURE) {
                briMultiplierInt = min((briMultiplierInt * 105) / 100, 100);  // Ensure it doesn't exceed 100.0 (represented as 1000)
                updateRequired = true;
                if (briMultiplierInt == 100){
                    shared->tempReduceCause = NO_CAUSE;
                }
            }

            if (updateRequired) {
                updateBrightnessCap(briMultiplierInt);
            }
            lastTempBriAdjust = millis();
        }
#endif
    }
}

#endif