//
// Created by Mohsen Basiti on 2025-04-02.
//

#pragma once
#ifdef USERMOD_CINEMAGIC_OLED

#include "cinemagic_shared.h"
#include <U8x8lib.h>
#include "cinemagic_fonts.c"


class CinemagicDisplay {
public:
    explicit CinemagicDisplay(CMShared *sh) : shared(sh) {};

    void begin();

    void loop();

private:
    CMShared *shared;
    U8X8_SSD1306_128X64_NONAME_HW_I2C *u8x8 = nullptr;
    unsigned long bootTime = 0;
    bool isDrawn = false;

    DisplayView lastView = NO_VIEW;
    DisplayMode lastMode = CCT_MODE;
    DisplayItem lastItem = NO_ITEM;
    bool lastAPMode = false;
    uint8_t lastBatPercentage = 0;
    int8_t lastTemp = 0;
    int32_t lastVoltage = 0;
    uint8_t lastBri = 0;
    uint8_t lastCCTTemp = 0;
    uint16_t lastHUE = 0;
    uint8_t lastSpeed = 0;
    uint8_t lastEffect = 0;
    uint8_t lastPallet = 0;

    void showStartupScreen();

    void showStatusBar();

    void showMainView();

    void showNetworkView();

    String getEffectOrPalletName(int inputEffPal, const char *qstring);
};

void CinemagicDisplay::begin() {
    u8x8 = new U8X8_SSD1306_128X64_NONAME_HW_I2C();
    u8x8->begin();
    // Flip if upside down
    u8x8->setFlipMode(1);

    bootTime = millis();
}

void CinemagicDisplay::loop() {
    // 1) After 4s on startup screen, switch to main view
    if (lastView != shared->currentView) {
        isDrawn = false;
        lastView = shared->currentView;
    } else {
        isDrawn = true;
    }
    switch (shared->currentView) {
        default:
        case STARTUP_VIEW:
            if (shared->currentView == STARTUP_VIEW && millis() - bootTime >= 4000) {
                showMainView();
                shared->currentView = MAIN_VIEW;
            } else {
                showStartupScreen();
            }
            break;
        case MAIN_VIEW:
            showMainView();
            break;
        case BATTERY_VIEW:
            break;
        case TEMPERATURE_VIEW:
            break;
        case NETWORK_VIEW:
            break;
        case SETTING_VIEW:
            break;
        case NODE_VIEWS:
            break;
    }

}

void CinemagicDisplay::showStartupScreen() {
    if (!isDrawn) {
        u8x8->clearDisplay();
        u8x8->setCursor(0, 0);
        u8x8->setFont(u8x8_font_torussansbold8_r);
        u8x8->draw1x2String(3, 2, "CINEMAGIC");
        u8x8->setFont(u8x8_font_chroma48medium8_r);
        u8x8->setCursor(3, 4);
        u8x8->println("Starting");
    }

    unsigned long elapsed = millis() - bootTime;
    if (elapsed >= 3000) {
        u8x8->setCursor(13, 4);
        u8x8->println(".");
    } else if (elapsed >= 2000) {
        u8x8->setCursor(12, 4);
        u8x8->println(".");
    } else if (elapsed >= 1000) {
        u8x8->setCursor(11, 4);
        u8x8->println(".");
    }
}

void CinemagicDisplay::showStatusBar() {
    u8x8->setFont(u8x8_font_chroma48medium8_r);

    // show battery percentage
    if (!isDrawn || lastBatPercentage != shared->power.batteryPercentage) {
        lastBatPercentage = shared->power.batteryPercentage;
        u8x8->setCursor(0, 0);
        if (shared->power.batteryPercentage >= 100) {
            u8x8->printf("%3u%%", shared->power.batteryPercentage);
        } else if (shared->power.batteryPercentage >= 10) {
            u8x8->printf("%2u%% ", shared->power.batteryPercentage);
        } else {
            u8x8->printf("%1u%%  ", shared->power.batteryPercentage);
        }
    }

    if (!isDrawn || lastVoltage != shared->power.voltage) {
        lastVoltage = shared->power.voltage;
        u8x8->setCursor(5, 0);
        u8x8->printf("%d.%02dV", shared->power.voltage / 100, abs(shared->power.voltage % 100));
    }

    // temperature
    if (!isDrawn || lastTemp != shared->temp.max) {
        lastTemp = shared->temp.max / 100;
        u8x8->setCursor(9, 0);
        u8x8->printf("%3u", lastTemp);
        u8x8->drawTile(12, 0, 1, (uint8_t *) CINEMAGIC_DP_DEGREE_ICON);
    }

    // wifi icon
    if (!isDrawn || lastAPMode != shared->apMode) {
        lastAPMode = shared->apMode;
        if (shared->apMode) {
            u8x8->setCursor(13, 0);
            u8x8->print("AP");
        } else {
            u8x8->setFont(CINEMAGIC_DP_Icons_1x1);
            u8x8->drawGlyph(13, 0, 19); // wifi icon
        }
    }

    // power icon
    u8x8->setFont(CINEMAGIC_DP_Icons_1x1);
    u8x8->drawGlyph(15, 0, (bri > 0 ? 9 : 0)); // power icon

    // bottom line
    if (!isDrawn) {
        for (uint8_t x = 0; x < 16; x++) {
            u8x8->drawTile(x, 1, 1, (uint8_t *) CINEMAGIC_DP_Line);
        }
    }
}

void CinemagicDisplay::showMainView() {
    if (!isDrawn) {
        u8x8->clearDisplay();
    }
    showStatusBar();

    u8x8->setCursor(0, 10);
    u8x8->setFont(u8x8_font_chroma48medium8_r);

    if (!isDrawn || lastMode != shared->currentMode) {
        lastMode = shared->currentMode;
        isDrawn = false;
        u8x8->setInverseFont(lastMode == CCT_MODE ? 1 : 0);
        u8x8->print(" CCT ");
        u8x8->setInverseFont(lastMode == HSI_MODE ? 1 : 0);
        u8x8->print(" HSI ");
        u8x8->setInverseFont(lastMode == EFFECT_MODE ? 1 : 0);
        u8x8->print(" FX ");
        u8x8->setInverseFont(lastMode == PRESET_MODE ? 1 : 0);
        u8x8->print("PS");
        u8x8->setInverseFont(0);
    }

    // Show brightness
    if (!isDrawn || lastBri != bri) {
        lastBri = bri;
        if (!isDrawn) {
            u8x8->setFont(CINEMAGIC_DP_Icons_2x1);
            u8x8->drawGlyph(1, 12, 1); // contrast Icon
        }
        u8x8->setCursor(4, 12);
        u8x8->setFont(u8x8_font_chroma48medium8_r);
        u8x8->printf("%u", (uint8_t) ((lastBri * 100) / 255));
    }

    // Show CCT Temperature
    if ((!isDrawn || lastCCTTemp != shared->ledCCTTemp) && lastMode == CCT_MODE) {
        lastCCTTemp = shared->ledCCTTemp;
        if (!isDrawn) {
            u8x8->setFont(CINEMAGIC_DP_Icons_2x1);
            u8x8->drawGlyph(1, 14, 8); // Sun Icon
        }
        u8x8->setCursor(4, 14);
        u8x8->setFont(u8x8_font_chroma48medium8_r);
        u8x8->printf("%u", (uint16_t) ((lastCCTTemp * (COLDEST_KELVIN - WARMEST_KELVIN) / 100) + WARMEST_KELVIN));
    }

    // Show HSI
    if ((!isDrawn || lastHUE != shared->ledHue) && lastMode == HSI_MODE) {
        lastHUE = shared->ledHue;
        if (!isDrawn) {
            u8x8->setFont(CINEMAGIC_DP_Icons_2x1);
            u8x8->drawGlyph(1, 14, 4); // Sun Icon
        }
        u8x8->setCursor(4, 14);
        u8x8->setFont(u8x8_font_chroma48medium8_r);
        u8x8->printf("%u%s", (uint8_t) (lastHUE), (lastHUE < 10 ? "   " : (lastHUE < 100 ? "  " : " ")));
    }

    // Show Speed
    if ((!isDrawn || lastSpeed != effectSpeed) && lastMode == EFFECT_MODE) {
        lastSpeed = effectSpeed;
        if (!isDrawn) {
            u8x8->setFont(CINEMAGIC_DP_Icons_2x1);
            u8x8->drawGlyph(1, 14, 2); // Speed Icon
        }
        u8x8->setCursor(4, 14);
        u8x8->setFont(u8x8_font_chroma48medium8_r);
        u8x8->printf("%u%s", (uint8_t) (lastSpeed), (lastSpeed < 10 ? "   " : (lastSpeed < 100 ? "  " : " ")));
    }

    // Show Effect
    if ((!isDrawn || lastEffect != effectCurrent) && lastMode == EFFECT_MODE) {
        lastEffect = effectCurrent;
        u8x8->setCursor(9, 12);
        u8x8->setFont(u8x8_font_chroma48medium8_r);
        u8x8->print(getEffectOrPalletName(lastEffect, JSON_mode_names));
    } else if (!isDrawn && lastMode != EFFECT_MODE){
        u8x8->setCursor(9, 12);
        u8x8->print("       ");
    }

    // Show Pallet
    if ((!isDrawn || lastPallet != effectPalette) && lastMode == EFFECT_MODE) {
        lastPallet = effectPalette;
        u8x8->setCursor(9, 14);
        u8x8->setFont(u8x8_font_chroma48medium8_r);
        u8x8->print(getEffectOrPalletName(lastPallet, JSON_palette_names));
    } else if (!isDrawn && lastMode != EFFECT_MODE){
        u8x8->setCursor(9, 14);
        u8x8->print("       ");
    }

    if (!isDrawn || lastItem != shared->currentItem) {
        lastItem = shared->currentItem;
        u8x8->setFont(CINEMAGIC_DP_Icons_1x1);

        switch (shared->currentItem) {
            default:
            case NO_ITEM:
                break;
            case BRIGHTNESS:
                u8x8->drawGlyph(0, 12, 20); // Play Icon
                u8x8->drawGlyph(8, 12, 0);
                u8x8->drawGlyph(0, 14, 0);
                u8x8->drawGlyph(8, 14, 0);
                break;
            case HSI:
            case EFFECT_SPEED:
            case COLOR_TEMPERATURE:
                u8x8->drawGlyph(0, 12, 0);
                u8x8->drawGlyph(8, 12, 0);
                u8x8->drawGlyph(0, 14, 20); // Play Icon
                u8x8->drawGlyph(8, 14, 0);
                break;
            case EFFECT:
                u8x8->drawGlyph(0, 12, 0);
                u8x8->drawGlyph(8, 12, 20);
                u8x8->drawGlyph(0, 14, 0);
                u8x8->drawGlyph(8, 14, 0);
                break;
            case PALETTE:
                u8x8->drawGlyph(0, 12, 0);
                u8x8->drawGlyph(8, 12, 0);
                u8x8->drawGlyph(0, 14, 0);
                u8x8->drawGlyph(8, 14, 20);
                break;
        }
    }
}

void CinemagicDisplay::showNetworkView() {
    u8x8->clearDisplay();
    u8x8->setCursor(0, 0);

    // Check if WLED is in AP or STA mode
    // A typical approach is:
    bool inApMode = (WiFi.getMode() & WIFI_AP);

    if (inApMode) {
        // Show AP SSID & pass
        // By default, WLED sets the AP credentials from wifi_settings.
        // Usually stored in global char apSSID[], apPass[] (in wled.h).

        u8x8->println("AP Mode!");
        u8x8->print("SSID: ");
        u8x8->println(shared->ssid);
        u8x8->print("Pass: ");
        u8x8->println(apPass);
    } else {
        // STA mode => show Wi-Fi SSID + IP
        // WiFi.SSID() is the station SSID
        // WiFi.localIP() is the IP
        u8x8->println("WiFi Mode");
        u8x8->print("SSID: ");
        u8x8->println(shared->ssid);

        u8x8->print("IP: ");
        u8x8->println(shared->ip.toString().c_str());
    }
}

String CinemagicDisplay::getEffectOrPalletName(int inputEffPal, const char *qstring) {
    char lineBuffer[MAX_JSON_CHARS];
    // Find the mode name in JSON
    uint8_t printedChars = extractModeName(inputEffPal, qstring, lineBuffer, MAX_JSON_CHARS - 1);
    if (lineBuffer[0] == '*' && lineBuffer[1] == ' ') {
        // remove "* " from dynamic palettes
        for (byte i = 2; i <= printedChars; i++) lineBuffer[i - 2] = lineBuffer[i]; //include '\0'
        printedChars -= 2;
    } else if ((lineBuffer[0] == ' ' && lineBuffer[1] > 127)) {
        // remove note symbol from effect names
        for (byte i = 5; i <= printedChars; i++) lineBuffer[i - 5] = lineBuffer[i]; //include '\0'
        printedChars -= 5;
    }
    char smallBuffer3[MAX_MODE_LINE_SPACE + 1];          // uses 1x1 icon for mode/palette
    uint8_t smallChars3 = 0;
    for (uint8_t i = 0; i < MAX_MODE_LINE_SPACE; i++)
        smallBuffer3[smallChars3++] = (i >= printedChars) ? ' ' : lineBuffer[i];
    smallBuffer3[smallChars3] = 0;
    return smallBuffer3;
}

#endif