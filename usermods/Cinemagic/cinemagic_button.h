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
    std::function<void()> onAnyButtonPressed;

    explicit CMButton(CMShared *sh) : shared(sh) {}

    void begin();

    void loop();

private:
    CMShared *shared;
    bool inited = false;
    static const uint8_t numBtn{CM_NUM_BTN};
    const uint8_t btnPin[CM_NUM_BTN] = {CINEMAGIC_BTN1_PIN, CINEMAGIC_BTN2_PIN, CINEMAGIC_BTN3_PIN};
    CMButtonState btnState[CM_NUM_BTN] = {NOT_PRESSED};
    unsigned long lastPressTime[CM_NUM_BTN] = {0};
    unsigned long buttonPressTime[CM_NUM_BTN] = {0};
    unsigned long lastHoldTime = {0};
    int currentItemIndex = 0;

    void handleButton(uint8_t b);
    void handleSinglePress(uint8_t b);
    void handleDoublePress(uint8_t b);
    void handleLongPress(uint8_t b);

    void setMode(DisplayMode mode);
    void setNextItemIndex();
    void setItemValue(byte ratio, bool increase);
    void setColorTemperature(uint8_t percent);
    void setHue(uint16_t hueDeg);

    void forceEnableAP();
};

void CMButton::begin(){
    // Initialize buttons
    for (uint8_t i = 0; i < numBtn; i++) {
        if (PinManager::allocatePin(btnPin[i], false, PinOwner::UM_CINEMAGIC)) {
            pinMode(btnPin[i], INPUT_PULLUP);
        }
    }

}

void CMButton::loop(){
    if (!inited){
        inited = true;
        setMode(shared->currentMode);
    }
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
            if (onAnyButtonPressed) onAnyButtonPressed();
            lastHoldTime = now;
        } else if (btnState[b] == LONG_PRESSED && now - lastHoldTime > 250 && b != 0){
            handleLongPress(b);
        }
    } else {
        if (btnState[b] == PRESSED) {
            if (now - buttonPressTime[b] < 500) {
                if (lastPressTime[b] != 0 && now - lastPressTime[b] < 500) {
                    handleDoublePress(b);
                    if (onAnyButtonPressed) onAnyButtonPressed();
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
        if (onAnyButtonPressed) onAnyButtonPressed();
        lastPressTime[b] = 0;
    }

}

void CMButton::handleSinglePress(uint8_t b) {
    DEBUG_PRINTF("Current View: %d \tButton No.%d Single Press!\n", shared->currentView, b);
    switch (shared->currentView) {
        default:
        case MAIN_VIEW:
            if (b == 1) {
                setItemValue(2, true);
            } else if (b == 2) {
                setItemValue(2, false);
            } else if (b == 0) {
                setNextItemIndex();
            }
            break;
    }
}

void CMButton::handleDoublePress(uint8_t b) {
    DEBUG_PRINTF("Button No.%d Double Press!\n", b);
    switch (shared->currentView){
        default:
        case MAIN_VIEW:
            if (b == 1) {
                setItemValue(5, true);
            } else if (b == 2) {
                setItemValue(5, false);
            } else if (b == 0){
                forceEnableAP();
            }
            break;
    }
}

void CMButton::handleLongPress(uint8_t b) {
    DEBUG_PRINTF("Button No.%d Long Press!\n", b);
    switch (shared->currentView) {
        default:
        case MAIN_VIEW:
            if (b == 1) {
                setItemValue(1, true);
            } else if (b == 2) {
                setItemValue(1, false);
            } else if (b == 0) {
                // long press menu btn: mode selection
                switch (shared->currentMode) {
                    default:
                    case CCT_MODE:
                        setMode(HSI_MODE);
                        break;
                    case HSI_MODE:
                        setMode(EFFECT_MODE);
                        break;
                    case EFFECT_MODE:
                        setMode(PRESET_MODE);
                        break;
                    case PRESET_MODE:
                        setMode(CCT_MODE);
                        break;
                }
            }
            break;
    }
}

void CMButton::setMode(DisplayMode mode){
    shared->currentMode = mode;
    currentItemIndex = 0;
    switch (shared->currentMode) {
        default:
        case CCT_MODE:
            effectCurrent = FX_MODE_STATIC;
            effectPalette = 0;
            setColorTemperature(shared->ledCCTTemp);
            colorUpdated(CALL_MODE_DIRECT_CHANGE);
            strip.show();
            delay(150);
            shared->currentItem = BRIGHTNESS;
            break;
        case HSI_MODE:
            effectCurrent = FX_MODE_STATIC;
            effectPalette = 0;
            setHue(shared->ledHue);
            colorUpdated(CALL_MODE_DIRECT_CHANGE);
            strip.show();
            delay(150);
            shared->currentItem = BRIGHTNESS;
            break;
        case EFFECT_MODE:
            shared->currentItem = BRIGHTNESS;
            break;
        case PRESET_MODE:
            shared->currentItem = BRIGHTNESS;
            break;
    }
}

void CMButton::setNextItemIndex() {
    DEBUG_PRINTF("CINEMAGIC Button: setNextItemIndex!\n");

    // cycle forward
    for (int i = 1; i < MAX_MODE_ITEMS; i++) {
        int nxt = (currentItemIndex + i) % MAX_MODE_ITEMS;
        if (DisplayModeItems[shared->currentMode][nxt] != NO_ITEM) {
            currentItemIndex = nxt;
            shared->currentItem = DisplayModeItems[shared->currentMode][nxt];
            DEBUG_PRINTF("CINEMAGIC Button: setNextItemIndex Selected: %d!\n", shared->currentItem);
            return;
        }
    }
}

void CMButton::setItemValue(byte ratio = 1, bool increase = true){
    switch (shared->currentItem) {
        default:
        case BRIGHTNESS:
            if (increase){
                bri = min(255, bri + 3 * ratio);
            } else {
                bri = max(0, bri - 3 * ratio);
            }
            strip.setBrightness(bri);
            colorUpdated(CALL_MODE_DIRECT_CHANGE);
            strip.show();
            delay(150);
            break;
        case COLOR_TEMPERATURE:
            if (increase){
                shared->ledCCTTemp = min(100, shared->ledCCTTemp + 1 * ratio);
            } else {
                shared->ledCCTTemp = max(0, shared->ledCCTTemp - 1 * ratio);
            }
            setColorTemperature(shared->ledCCTTemp);
            colorUpdated(CALL_MODE_DIRECT_CHANGE);
            strip.show();
            delay(150);
            break;
        case HSI:
            if (increase){
                shared->ledHue = min(360, shared->ledHue + 1 * ratio);
                if (shared->ledHue == 360){
                    shared->ledHue = 0;
                }
            } else {
                shared->ledHue = max(0, shared->ledHue - 1 * ratio);
                if (shared->ledHue == 0){
                    shared->ledHue = 360;
                }
            }
            setHue(shared->ledHue);
            colorUpdated(CALL_MODE_DIRECT_CHANGE);
            strip.show();
            delay(150);
            break;
    }
}

void CMButton::setColorTemperature(uint8_t percent)
{
    DEBUG_PRINTF("Button setColorTemperature: %d\n", percent);
    // 1) Convert 0..100% => Kelvin
    uint16_t kelvin = WARMEST_KELVIN
                      + (uint32_t)(COLDEST_KELVIN - WARMEST_KELVIN) * percent / 100;

    // 2) Get the first bus
    Bus* bus = BusManager::getBus(0);
    if (!bus) {
        // no bus? just exit
        return;
    }

    // 3) Determine how many color channels
    uint8_t chanCount = (uint8_t)bus->getNumberOfChannels();
    // e.g. 2 => CCT, 3 => RGB, 4 => RGBW, 5 => RGB + CCT

    // 4) Clear all possible col[] channels up to 5
    for (uint8_t i = 0; i < 5; i++) {
        col[i] = 0;
    }

    if (chanCount == 2) {
        // **Dedicated CCT** (warm + cold).
        // We'll assume col[0] = warm, col[1] = cold.
        //  0% => fully warm => col[0]=255, col[1]=0
        //  100% => fully cold => col[0]=0, col[1]=255
        uint8_t warmVal = 255 - map(percent, 0, 100, 0, 255);
        uint8_t coldVal = map(percent, 0, 100, 0, 255);
        col[0] = warmVal;
        col[1] = coldVal;
    }
    else if (chanCount == 3) {
        // **RGB**
        // Use colorKtoRGB to get an approximate white color
        byte rgb[4];
        colorKtoRGB(kelvin, rgb); // rgb[] => {r,g,b,0}
        col[0] = rgb[0];
        col[1] = rgb[1];
        col[2] = rgb[2];
    }
    else if (chanCount == 4) {
        // **RGBW**
        byte rgb[4];
        colorKtoRGB(kelvin, rgb);  // {r,g,b,0}
        col[0] = rgb[0];
        col[1] = rgb[1];
        col[2] = rgb[2];
        // Simple approach: fill white channel with average of R,G,B
        col[3] = (rgb[0] + rgb[1] + rgb[2]) / 3;
    }
    else if (chanCount == 5) {
        // e.g. "RGB + dual white" (warm + cold)
        // We'll assume col[0..2] => R,G,B, col[3] => warm, col[4] => cold
        // We'll set RGB=0, use the same logic as the 2-channel CCT
        uint8_t warmVal = 255 - map(percent, 0, 100, 0, 255);
        uint8_t coldVal = map(percent, 0, 100, 0, 255);
        col[0] = 0;  // R
        col[1] = 0;  // G
        col[2] = 0;  // B
        col[3] = warmVal;
        col[4] = coldVal;
    } else {
        // fallback or unknown config
        // If chanCount < 2 or > 5, do something safe
    }
}

void CMButton::setHue(uint16_t hueDeg)
{
    DEBUG_PRINTF("Button setHue: %d\n", hueDeg);
    // 1) Clamp and convert to 0..65535
    if (hueDeg > 360) hueDeg = 360;
    // 360° ~ 65535, so multiply by ~182
    uint16_t hue65535 = (uint32_t)hueDeg * 182;

    // 2) Get the first bus
    Bus* bus = BusManager::getBus(0);
    if (!bus) {
        // no bus? exit
        return;
    }

    // 3) Number of color channels
    uint8_t chanCount = (uint8_t)bus->getNumberOfChannels();
    // e.g. 2 => dedicated CCT, 3 => RGB, 4 => RGBW, 5 => “RGB + CCT”

    // 4) Clear all possible col[] channels up to 5
    for (uint8_t i = 0; i < 5; i++) {
        col[i] = 0;
    }

    // 5) Convert (Hue,S=255) to RGB
    //    colorHStoRGB() sets R,G,B in [0..2], alpha not used
    byte rgbTmp[3];
    colorHStoRGB(hue65535, 255, rgbTmp); // full saturation, full brightness

    // 6) Depending on bus channels, set col[]
    if (chanCount == 2) {
        // **2-channel CCT** (warm + cold).
        // We only have grayscale. Easiest approach:
        // compute luminance from the resulting color, then map it to warm/cold = half/half or just total brightness => warm?
        // We'll do a quick approach that sets fully on if not black, ignoring hue.
        uint16_t luma = (77UL * rgbTmp[0] + 150UL * rgbTmp[1] + 29UL * rgbTmp[2]) >> 8;
        // You could convert that to a warm/cold ratio, but that's up to your design.
        col[0] = luma; // warm
        col[1] = 0;    // cold
    }
    else if (chanCount == 3) {
        // **RGB**
        col[0] = rgbTmp[0];
        col[1] = rgbTmp[1];
        col[2] = rgbTmp[2];
    }
    else if (chanCount == 4) {
        // **RGBW**
        col[0] = rgbTmp[0];
        col[1] = rgbTmp[1];
        col[2] = rgbTmp[2];
        // simple approach: fill col[3] (W) with average
        col[3] = (rgbTmp[0] + rgbTmp[1] + rgbTmp[2]) / 3;
    }
    else if (chanCount == 5) {
        // **RGB + Warm + Cold**
        // We'll set RGB from the hue, and maybe keep warm/cold at 0.
        // or do your own blending.
        col[0] = rgbTmp[0];
        col[1] = rgbTmp[1];
        col[2] = rgbTmp[2];
        col[3] = 0; // warm
        col[4] = 0; // cold
    } else {
        // fallback or unknown config
    }

}

void CMButton::forceEnableAP()
{
    // 1) Optionally stop any STA attempts
    //    This makes sure we won't reconnect to a router.
    WiFi.disconnect(true);

    // 2) Set Wi-Fi to AP-only mode
#if defined(ARDUINO_ARCH_ESP32)
    WiFi.mode(WIFI_MODE_AP);
#else
    WiFi.mode(WIFI_AP);
#endif

    // 3) Provide SSID/password for the AP
    //    You can use your own strings or WLED’s configured values:
    //      serverDescription, apPass, etc.
    //    For example, if you want to re-use user settings from wled.h:
    WiFi.softAP(apSSID, apPass);

    // 4) Mark the AP as active
    apActive = true;

    // 5) (Optional) If you want WLED to remain in AP mode persistently,
    //    set apBehavior = AP_BEHAVIOR_ALWAYS (==3)
    apBehavior = AP_BEHAVIOR_ALWAYS;
}
#endif