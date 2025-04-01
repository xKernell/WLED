#pragma once

#include "wled.h"
#include "pin_manager.h"
#include "cinemagic_shared.h"

#ifdef USERMOD_CINEMAGIC_OLED
#include "cinemagic_display.h"
#endif

#ifdef USERMOD_CINEMAGIC_POWER
#include "cinemagic_power.h"
#endif

#ifdef USERMOD_CINEMAGIC_TEMPERATURE
#include "cinemagic_temperature.h"
#endif

#ifdef CINEMAGIC_WITH_3_BUTTON
#include "cinemagic_button.h"
#endif

#ifdef USERMOD_CINEMAGIC_BLE
#include "cinemagic_ble.h"
#endif

#ifdef USERMOD_CINEMAGIC_WITH_I2C_SCANNER
#include "I2CScanner.h"
I2CScanner scanner;
#endif


class UsermodCinemagic : public Usermod {
public:
    CMShared shared;

private:
#ifdef USERMOD_CINEMAGIC_OLED
    CinemagicDisplay display{&shared};
#endif
#ifdef USERMOD_CINEMAGIC_POWER
    CMPower power{&shared};
#endif
#ifdef USERMOD_CINEMAGIC_TEMPERATURE
    CMTemperature temperature{&shared};
#endif
#ifdef CINEMAGIC_WITH_3_BUTTON
    CMButton button{&shared};
#endif
#ifdef USERMOD_CINEMAGIC_BLE
    CMBLE ble{&shared};
#endif
#ifdef CINEMAGIC_FAN_PIN
    uint8_t lastFanDuty = 0; // 0-255
#endif
    bool enabled{true};
    bool mInited{false};


    // string that are used multiple time (this will save some flash memory)
    static const char _name[];
    static const char _enabled[];

    void saveToConfig();

public:
    /**
     * Enable/Disable the usermod
     */
    inline void enable(bool enable) { enabled = enable; }

    /**
     * Get usermod enabled/disabled state
     */
    inline bool isEnabled() { return enabled; }

    void setup() {
        if (!enabled) return;
        DEBUG_PRINTLN("Cinemagic user mod started");

#ifdef USERMOD_CINEMAGIC_OLED
        display.beginType2();
#endif

#ifdef USERMOD_CINEMAGIC_POWER
        power.begin();
#endif

#ifdef USERMOD_CINEMAGIC_WITH_I2C_SCANNER
        scanner.Init();
        delay(5000);
        scanner.Scan();
#endif

#ifdef USERMOD_CINEMAGIC_TEMPERATURE
        temperature.begin();
#endif

#ifdef CINEMAGIC_WITH_3_BUTTON
        button.begin();
#endif

#ifdef USERMOD_CINEMAGIC_BLE
        ble.begin();
#endif

#ifdef CINEMAGIC_FAN_PIN
    // Configure the LEDC PWM channel
    ledcSetup(1, 25000, 8);
    // Attach the channel to the pin
    ledcAttachPin(CINEMAGIC_FAN_PIN, 1);
#endif

        mInited = true;
    }


    /*
     * connected() is called every time the WiFi is (re)connected
     * Use it to initialize network interfaces
     */
    void connected() override {
        shared.ssid = apActive ? apSSID : WiFi.SSID();
        shared.ip = Network.localIP();
#ifdef USERMOD_CINEMAGIC_OLED
//        display.networkOverlay(PSTR("NETWORK INFO"), 7000);
#endif
    }

    void loop() {
        // if usermod is disabled or called during strip updating just exit
        // NOTE: on very long strips strip.isUpdating() may always return true so update accordingly
        if (!enabled || strip.isUpdating()) return;

//        // Max brightness mode handling
//        if (inMaxBrightnessMode) {
//            if (millis() - maxBrightnessStartTime > MAX_BRIGHTNESS_DURATION ||
//                shared.temp.board > USERMOD_CINEMAGIC_MAX_SAFE_TEMP * 100) {
//                // Reduce brightness
//                bri = previousBrightness;
//                strip.setBrightness(bri);
//                updateInterfaces(CALL_MODE_DIRECT_CHANGE);
//                inMaxBrightnessMode = false;
//            }
//        }

        // power measurement
#ifdef USERMOD_CINEMAGIC_POWER
        power.loop();
#endif

        // temperature
#ifdef USERMOD_CINEMAGIC_TEMPERATURE
        temperature.loop();
#endif

        // display update
#ifdef USERMOD_CINEMAGIC_OLED
        display.loop();
#endif

#ifdef CINEMAGIC_WITH_3_BUTTON
        button.loop();
#endif
#ifdef USERMOD_CINEMAGIC_BLE
        ble.loop();
#endif

#ifdef CINEMAGIC_FAN_PIN
        // Update only if changed
        int currentBrightness = bri ;
        int mappedDuty;
        if (currentBrightness <= 1) {
            // If brightness is 0, turn fan fully off
            mappedDuty = 0;
        } else {
            // Otherwise, map [1..255] brightness to [80..255] duty
            // (Adjust these values to taste)
            mappedDuty = map(currentBrightness, 2, 255, 190, 255);
        }

        if (mappedDuty != lastFanDuty) {
            lastFanDuty = mappedDuty;
            ledcWrite(1, mappedDuty);
        }
#endif

    }


    void onUpdateBegin(bool init);

    /*
     * addToJsonInfo() can be used to add custom entries to the /json/info part of the JSON API.
     * Creating an "u" object allows you to add custom key/value pairs to the Info section of the WLED web UI.
     * Below it is shown how this could be used for e.g. a light sensor
     */
    void addToJsonInfo(JsonObject &root) {
        // if "u" object does not exist yet wee need to create it
        JsonObject user = root["u"];
        if (user.isNull()) user = root.createNestedObject("u");

#ifdef USERMOD_CINEMAGIC_POWER
        JsonArray data = user.createNestedArray("Power");
        data.add(shared.power.displayStr);
#endif

#ifdef USERMOD_CINEMAGIC_TEMPERATURE
        // Add temp 1 to the JSON info
        JsonArray temp1Arr = user.createNestedArray(F("CPU Temp"));
        temp1Arr.add((float) shared.temp.cpu / 100.0f);
        temp1Arr.add(F("C"));

        // Add temp 1 to the JSON info
        if (USERMOD_CINEMAGIC_TEMP_LEDPANEL_PIN > -1) {
            JsonArray temp1Arr = user.createNestedArray(F("LED Panel Temp"));
            temp1Arr.add((float) shared.temp.led / 100.0f);
            temp1Arr.add(F("C"));
        }

        // Add temp 2 to the JSON info
        if (USERMOD_CINEMAGIC_TEMP_BOARD_PIN > -1) {
            JsonArray temp2Arr = user.createNestedArray(F("Mainboard Temp"));
            temp2Arr.add((float) shared.temp.board / 100.0f);
            temp2Arr.add(F("C"));
        }
#endif

        // Add temp 2 to the JSON info
        JsonArray temp4Arr = user.createNestedArray(F("Brightness Cause"));
        temp4Arr.add(shared.getTempReduceCauseString());

        // Add temp 2 to the JSON info
        JsonArray temp2Arr = user.createNestedArray(F("Brightness Limit"));
        temp2Arr.add(briMultiplier);
        temp2Arr.add(F("%"));
    }


    /*
     * addToJsonState() can be used to add custom entries to the /json/state part of the JSON API (state object).
     * Values in the state object may be modified by connected clients
     */
    void addToJsonState(JsonObject &root) {
        if (!mInited || !enabled) return;  // prevent crash on boot applyPreset()

        JsonObject usermod = root[FPSTR(_name)];
        if (usermod.isNull()) usermod = root.createNestedObject(FPSTR(_name));

        //usermod["user0"] = userVar0;
    }


    /*
     * readFromJsonState() can be used to receive data clients send to the /json/state part of the JSON API (state object).
     * Values in the state object may be modified by connected clients
     */
    void readFromJsonState(JsonObject &root) {
        if (!mInited) return;  // prevent crash on boot applyPreset()

        JsonObject usermod = root[FPSTR(_name)];
        if (!usermod.isNull()) {
            // expect JSON usermod data in usermod name object: {"ExampleUsermod:{"user0":10}"}
            userVar0 = usermod["user0"] | userVar0; //if "user0" key exists in JSON, update, else keep old value
        }
        // you can as well check WLED state JSON keys
        //if (root["bri"] == 255) Serial.println(F("Don't burn down your garage!"));
    }


    void addToConfig(JsonObject &root);

    bool readFromConfig(JsonObject &root);

    /*
     * handleOverlayDraw() is called just before every show() (LED strip update frame) after effects have set the colors.
     * Use this to blank out some LEDs or set them to a different color regardless of the set effect mode.
     * Commonly used for custom clocks (Cronixie, 7 segment)
     */
    void handleOverlayDraw() {
        //strip.setPixelColor(0, RGBW32(0,0,0,0)) // set the first pixel to black
    }

#ifndef WLED_DISABLE_MQTT

    /**
     * handling of MQTT message
     * topic only contains stripped topic (part after /wled/MAC)
     */
    bool onMqttMessage(char *topic, char *payload) {
        // check if we received a command
        //if (strlen(topic) == 8 && strncmp_P(topic, PSTR("/command"), 8) == 0) {
        //  String action = payload;
        //  if (action == "on") {
        //    enabled = true;
        //    return true;
        //  } else if (action == "off") {
        //    enabled = false;
        //    return true;
        //  } else if (action == "toggle") {
        //    enabled = !enabled;
        //    return true;
        //  }
        //}
        return false;
    }

    /**
     * onMqttConnect() is called when MQTT connection is established
     */
    void onMqttConnect(bool sessionPresent) {
        // do any MQTT related initialisation here
        //publishMqtt("I am alive!");
    }

#endif


    /**
     * onStateChanged() is used to detect WLED state change
     * @mode parameter is CALL_MODE_... parameter used for notifications
     */
    void onStateChange(uint8_t mode) {
        // do something if WLED state changed (color, brightness, effect, preset, etc)
    }


    /*
     * getId() allows you to optionally give your V2 usermod an unique ID (please define it in const.h!).
     * This could be used in the future for the system to determine whether your usermod is installed.
     */
    uint16_t getId() {
        return USERMOD_ID_EXAMPLE;
    }

    //More methods can be added in the future, this example will then be extended.
    //Your usermod will remain compatible as it does not need to implement all methods from the Usermod base class!
};


// add more strings here to reduce flash memory usage
const char UsermodCinemagic::_name[]            PROGMEM = "Cinemagic";
const char UsermodCinemagic::_enabled[]         PROGMEM = "enabled";

void UsermodCinemagic::onUpdateBegin(bool init) {
#ifdef USERMOD_CINEMAGIC_OLED
//    display.createTask(init);
#endif
}

/*
  * addToConfig() can be used to add custom persistent settings to the cfg.json file in the "um" (usermod) object.
  * It will be called by WLED when settings are actually saved (for example, LED settings are saved)
  * If you want to force saving the current state, use serializeConfig() in your loop().
  *
  * CAUTION: serializeConfig() will initiate a filesystem write operation.
  * It might cause the LEDs to stutter and will cause flash wear if called too often.
  * Use it sparingly and always in the loop, never in network callbacks!
  *
  * addToConfig() will also not yet add your setting to one of the settings pages automatically.
  * To make that work you still have to add the setting to the HTML, xml.cpp and set.cpp manually.
  *
  * I highly recommend checking out the basics of ArduinoJson serialization and deserialization in order to use custom settings!
  */
void UsermodCinemagic::addToConfig(JsonObject &root) {
    JsonObject top = root.createNestedObject(FPSTR(_name));

    top[FPSTR(_enabled)] = enabled;

#ifdef USERMOD_CINEMAGIC_OLED
//    JsonArray io_pin = top.createNestedArray("pin");
//    for (int i = 0; i < 3; i++) io_pin.add(display.ioPin[i]);
//    DEBUG_PRINTLN(F("4 Line Display config saved."));
//    top[FPSTR(display._flip)] = (bool) display.flip;
//    top[FPSTR(display._contrast)] = display.contrast;
//    top[FPSTR(display._contrastFix)] = (bool) display.contrastFix;
//    top[FPSTR(display._screenTimeOut)] = display.screenTimeout / 1000;
//    top[FPSTR(display._sleepMode)] = (bool) display.sleepMode;
//    top[FPSTR(display._clockMode)] = (bool) display.clockMode;
//    top[FPSTR(display._showSeconds)] = (bool) display.showSeconds;
#endif
}

/*
  * readFromConfig() can be used to read back the custom settings you added with addToConfig().
  * This is called by WLED when settings are loaded (currently this only happens once immediately after boot)
  *
  * readFromConfig() is called BEFORE setup(). This means you can use your persistent values in setup() (e.g. pin assignments, buffer sizes),
  * but also that if you want to write persistent values to a dynamic buffer, you'd need to allocate it here instead of in setup.
  * If you don't know what that is, don't fret. It most likely doesn't affect your use case :)
  */
bool UsermodCinemagic::readFromConfig(JsonObject &root) {
    JsonObject top = root[FPSTR(_name)];
    if (top.isNull()) {
        DEBUG_PRINT(FPSTR(_name));
        DEBUG_PRINTLN(F(": No config found. (Using defaults.)"));
        return false;
    }

    enabled = top[FPSTR(_enabled)] | enabled;

    // read display configurations
#ifdef USERMOD_CINEMAGIC_OLED
//    for (byte i = 0; i < 3; i++) display.ioPin[i] = top["pin"][i] | display.ioPin[i];
//    display.flip = top[FPSTR(display._flip)] | display.flip;
//    display.contrast = top[FPSTR(display._contrast)] | display.contrast;
//    display.screenTimeout = (top[FPSTR(display._screenTimeOut)] | display.screenTimeout / 1000) * 1000;
//    display.sleepMode = top[FPSTR(display._sleepMode)] | display.sleepMode;
//    display.clockMode = top[FPSTR(display._clockMode)] | display.clockMode;
//    display.showSeconds = top[FPSTR(display._showSeconds)] | display.showSeconds;
//    display.contrastFix = top[FPSTR(display._contrastFix)] | display.contrastFix;
//    // use "return !top["newestParameter"].isNull();" when updating Usermod with new features
//    return !top[FPSTR(display._contrastFix)].isNull();
#endif
    return true;
}

void UsermodCinemagic::saveToConfig() {
    // Trigger WLED to save the configuration including your variable
    serializeConfig();
}
