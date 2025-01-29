//
// Created by Mohsen Basiti on 2024-11-17.
//

#pragma once
#ifdef CINEMAGIC_WITH_3_BUTTON
#include "cinemagic_shared.h" // Include shared structures

#define CM_NUM_BTN 3 // btn 1 = menu, btn 2 = up, btn 3 = down

#ifndef CINEMAGIC_BTN1_PIN
#define CINEMAGIC_BTN1_PIN 13
#endif

#ifndef CINEMAGIC_BTN2_PIN
#define CINEMAGIC_BTN2_PIN 35
#endif

#ifndef CINEMAGIC_BTN3_PIN
#define CINEMAGIC_BTN3_PIN 34
#endif

#define CM_BTN_LONG_PRESS_TIME 1000
#define CM_BTN_DOUBLE_PRESS_TIME 500


enum CMButtonState {
    NOT_PRESSED,
    PRESSED,
    LONG_PRESSED,
    DOUBLE_PRESSED
};

class CMButton {
public:
    using Slot = void (*)();

    explicit CMButton(CMShared *sh) : shared(sh) {}

    void begin();

    void loop();

private:
    CMShared *shared;
    static const uint8_t numBtn{CM_NUM_BTN};
    const uint8_t btnPin[CM_NUM_BTN] = {CINEMAGIC_BTN1_PIN, CINEMAGIC_BTN2_PIN, CINEMAGIC_BTN3_PIN};
    CMButtonState btnState[CM_NUM_BTN] = {NOT_PRESSED};
    unsigned long lastPressTime[CM_NUM_BTN] = {0};
    unsigned long buttonPressTime[CM_NUM_BTN] = {0};

    void handleButton(uint8_t b);
    void handleSinglePress(uint8_t b);
    void handleDoublePress(uint8_t b);
    void handleLongPress(uint8_t b);
};

void CMButton::begin(){
    // Initialize buttons
    for (uint8_t i = 0; i < numBtn; i++) {
        if (pinManager.allocatePin(btnPin[i], false, PinOwner::UM_CINEMAGIC)) {
            pinMode(btnPin[i], INPUT_PULLUP);
        }
    }

}

void CMButton::loop(){
    for (uint8_t i = 0; i < numBtn; i++) {
        handleButton(i);
    }
}

void CMButton::handleButton(uint8_t b){
    if (b >= CM_NUM_BTN) return;
    bool isPressed = digitalRead(btnPin[b]) == LOW;
    unsigned long now = millis();

    if (isPressed) {
        if (btnState[b] == NOT_PRESSED) {
            buttonPressTime[b] = now;
            btnState[b] = PRESSED;
        } else if (btnState[b] == PRESSED && now - buttonPressTime[b] > 1000) {
            btnState[b] = LONG_PRESSED;
            handleLongPress(b);
        }
    } else {
        if (btnState[b] == PRESSED) {
            if (now - buttonPressTime[b] < 500) {
                if (lastPressTime[b] != 0 && now - lastPressTime[b] < 500) {
                    handleDoublePress(b);
                    lastPressTime[b] = 0;
                } else {
                    lastPressTime[b] = now;
                }
            }
        }
        btnState[b] = NOT_PRESSED;
    }

    // Handle single press after interval
    if (lastPressTime[b] != 0 && now - lastPressTime[b] > 500) {
        handleSinglePress(b);
        lastPressTime[b] = 0;
    }
}

void CMButton::handleSinglePress(uint8_t b) {
//    switch (b) {
//        case 0: // Button A
//            if (shared.currentView == SETTINGS_VIEW) {
//                // Exit settings mode
//                shared.currentView = previousView;
//#ifdef USERMOD_CINEMAGIC_OLED
//                display.updateDisplay(shared.currentView);
//#endif
//            } else {
//                navigateToNextView();
//            }
//            break;
//        case 1: // Button B
//            if (shared.currentView == SETTINGS_VIEW) {
//#ifdef USERMOD_CINEMAGIC_OLED
//                // Toggle sleepMode
//                display.updateSettings(!display.sleepMode, display.screenTimeout);
//                display.updateDisplay(shared.currentView);
//#endif
//            } else {
//                adjustCurrentSetting(+1);
//            }
//            break;
//        case 2: // Button C
//            if (shared.currentView == SETTINGS_VIEW) {
//#ifdef USERMOD_CINEMAGIC_OLED
//                // Increase timeout duration
//                if (display.sleepMode) {
//                    uint32_t newTimeout = display.screenTimeout + 30000; // Increase by 30 seconds
//                    display.updateSettings(display.sleepMode, newTimeout);
//                    display.updateDisplay(shared.currentView);
//                }
//#endif
//            } else {
//                adjustCurrentSetting(-1);
//            }
//            break;
//    }
}

void CMButton::handleDoublePress(uint8_t b) {
//    switch (b) {
//        case 0: // Button A
//            navigateToPreviousView();
//            break;
//        case 1: // Button B
//            adjustCurrentSetting(+10);
//            break;
//        case 2: // Button C
//            adjustCurrentSetting(-10);
//            break;
//    }
}

void CMButton::handleLongPress(uint8_t b) {
//    switch (b) {
//        case 0: // Button A
//            // Enter settings mode
//            previousView = shared.currentView;
//            shared.currentView = SETTINGS_VIEW;
//#ifdef USERMOD_CINEMAGIC_OLED
//            display.updateDisplay(shared.currentView);
//#endif
//            break;
//        case 1: // Button B
//            // Implement continuous increase if needed
//            break;
//        case 2: // Button C
//            // Implement continuous decrease if needed
//            break;
//    }
}


//
//uint8_t selectedModeIndex = 0; // For mode selection
//DisplayView previousView = MAIN_VIEW;
//
//// Max brightness mode
//bool inMaxBrightnessMode = false;
//unsigned long maxBrightnessStartTime = 0;
//const unsigned long MAX_BRIGHTNESS_DURATION = 30000; // 30 seconds
//uint8_t previousBrightness = 0;
//
//// Helper methods
//void navigateToNextView();
//
//void navigateToPreviousView();
//
//void enterSettingsMode();
//
//void adjustCurrentSetting(int8_t direction);
//
//void startContinuousAdjustment(int8_t direction);
//
//void activateMaxBrightnessMode();
//
//
//void UsermodCinemagic::navigateToNextView() {
//    shared.currentView = static_cast<DisplayView>((shared.currentView + 1) % TOTAL_VIEWS);
//#ifdef USERMOD_CINEMAGIC_OLED
//    display.updateDisplay(shared.currentView);
//#endif
//}
//
//void UsermodCinemagic::navigateToPreviousView() {
//    shared.currentView = static_cast<DisplayView>((shared.currentView - 1 + TOTAL_VIEWS) % TOTAL_VIEWS);
//#ifdef USERMOD_CINEMAGIC_OLED
//    display.updateDisplay(shared.currentView);
//#endif
//}
//
//void UsermodCinemagic::enterSettingsMode() {
//    previousView = shared.currentView;
//    shared.currentView = SETTINGS_VIEW;
//#ifdef USERMOD_CINEMAGIC_OLED
//    display.updateDisplay(shared.currentView);
//#endif
//}
//
//void UsermodCinemagic::adjustCurrentSetting(int8_t direction) {
//    if (shared.currentView == SETTINGS_VIEW) {
//        // Handled in handleSinglePress
//        return;
//    }
//    switch (shared.currentView) {
//        case MAIN_VIEW:
//            // Adjust brightness
//            bri = constrain(bri + direction * 5, 0, 255);
//            strip.setBrightness(bri);
//            updateInterfaces(CALL_MODE_DIRECT_CHANGE);
//#ifdef USERMOD_CINEMAGIC_OLED
//            display.updateBrightness(bri);
//#endif
//            break;
//        case MODE_SELECTION_VIEW:
//            // Navigate modes
//            selectedModeIndex = (selectedModeIndex + direction + 3) % 3;
//#ifdef USERMOD_CINEMAGIC_OLED
//            display.updateModeSelection(selectedModeIndex);
//#endif
//            break;
//            // Handle other views if needed
//        default:
//            break;
//    }
//}
//
//void UsermodCinemagic::activateMaxBrightnessMode() {
//    previousBrightness = bri;
//    bri = 255;
//    strip.setBrightness(bri);
//    updateInterfaces(CALL_MODE_DIRECT_CHANGE);
//    maxBrightnessStartTime = millis();
//    inMaxBrightnessMode = true;
//}

#endif