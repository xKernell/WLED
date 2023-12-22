#pragma once

#include "wled.h"
#include "cinemagic_display.h"

// RGB PWM configs
#ifndef USERMOD_CINEMAGIC_RGB_PWM_PINS
#define USERMOD_CINEMAGIC_RGB_PWM_PINS 2
#endif

class UsermodCinemagic : public Usermod {
private:
    CinemagicDisplay display;
    bool enabled{true};
    bool mInited{false};

    // Next variables hold the previous known values to determine if redraw is
    // required.

    // string that are used multiple time (this will save some flash memory)
    static const char _name[];
    static const char _enabled[];

    // RGB PWM variables
    bool mRGBPWNinited{false};
    bool mRGBPWNState{false};
    uint8_t mRGBPWMChannel{255};
    unsigned long lastupdate{0};

    void initRGBPWM() {
        DEBUG_PRINTF("cinemagic_pwm_output[%d]: setup to freq %d\n", USERMOD_CINEMAGIC_RGB_PWM_PINS, 5000);

        if (mRGBPWNinited)
            return;

        if (!pinManager.allocatePin(USERMOD_CINEMAGIC_RGB_PWM_PINS, true, PinOwner::UM_PWM_OUTPUTS))
            return;

        mRGBPWMChannel = pinManager.allocateLedc(1);
        if (mRGBPWMChannel == 255) {
            pinManager.deallocatePin(USERMOD_CINEMAGIC_RGB_PWM_PINS, PinOwner::UM_PWM_OUTPUTS);
            return;
        }

        ledcSetup(mRGBPWMChannel, 5000, 8);
        ledcAttachPin(USERMOD_CINEMAGIC_RGB_PWM_PINS, mRGBPWMChannel);

        mRGBPWNinited = true;
        DEBUG_PRINTLN(F("cinemagic_pwm_output inited"));
    }

    void closeRGBPWM() {
        pinManager.deallocatePin(USERMOD_CINEMAGIC_RGB_PWM_PINS, PinOwner::UM_PWM_OUTPUTS);
        if (mRGBPWMChannel != 255)
            pinManager.deallocateLedc(mRGBPWMChannel, 1);
        mRGBPWMChannel = 255;
        mRGBPWNinited = false;
    }

    void turnOnRGB() {
        if (!mRGBPWNinited || mRGBPWNState)
            return;

        ledcWrite(mRGBPWMChannel, 255);
        mRGBPWNState = true;
        DEBUG_PRINTLN("cinemagic_pwm_output ON");
    }

    void turnOffRGB() {
        if (!mRGBPWNinited || !mRGBPWNState)
            return;

        ledcWrite(mRGBPWMChannel, 0);
        mRGBPWNState = false;
        DEBUG_PRINTLN("cinemagic_pwm_output OFF");
    }
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

        initRGBPWM();
        display.begin();
        mInited = true;
    }


    /*
     * connected() is called every time the WiFi is (re)connected
     * Use it to initialize network interfaces
     */
    void connected() {
        display.knownSsid = WiFi.SSID();       //apActive ? apSSID : WiFi.SSID(); //apActive ? WiFi.softAPSSID() :
        display.knownIp = Network.localIP(); //apActive ? IPAddress(4, 3, 2, 1) : Network.localIP();
        display.networkOverlay(PSTR("NETWORK INFO"), 7000);
    }

    void loop() {
        // if usermod is disabled or called during strip updating just exit
        // NOTE: on very long strips strip.isUpdating() may always return true so update accordingly
        if (!enabled || strip.isUpdating()) return;

//        if (millis() - lastupdate > 2000) {
            if (!mRGBPWNState) {
                turnOnRGB();
            }
//            } else {
//                turnOffRGB();
//            }
//            lastupdate = millis();
//        }

        display.loop();
    }

    /**
     * handleButton() can be used to override default button behaviour. Returning true
     * will prevent button working in a default way.
     * Replicating button.cpp
     */
    bool handleButton(uint8_t b);

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

        //this code adds "u":{"ExampleUsermod":[20," lux"]} to the info object
        //int reading = 20;
        //JsonArray lightArr = user.createNestedArray(FPSTR(_name))); //name
        //lightArr.add(reading); //value
        //lightArr.add(F(" lux")); //unit

        // if you are implementing a sensor usermod, you may publish sensor data
        //JsonObject sensor = root[F("sensor")];
        //if (sensor.isNull()) sensor = root.createNestedObject(F("sensor"));
        //temp = sensor.createNestedArray(F("light"));
        //temp.add(reading);
        //temp.add(F("lux"));
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

    void appendConfigData();


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

/**
 * handleButton() can be used to override default button behaviour. Returning true
 * will prevent button working in a default way.
 * Replicating button.cpp
 */
bool UsermodCinemagic::handleButton(uint8_t b) {
    yield();
    if (!enabled
        || b // button 0 only
        || buttonType[b] == BTN_TYPE_SWITCH
        || buttonType[b] == BTN_TYPE_NONE
        || buttonType[b] == BTN_TYPE_RESERVED
        || buttonType[b] == BTN_TYPE_PIR_SENSOR
        || buttonType[b] == BTN_TYPE_ANALOG
        || buttonType[b] == BTN_TYPE_ANALOG_INVERTED) {
        return false;
    }

    unsigned long now = millis();
    static bool buttonPressedBefore = false;
    static bool buttonLongPressed = false;
    static unsigned long buttonPressedTime = 0;
    static unsigned long buttonWaitTime = 0;
    bool handled = false;

    //momentary button logic
    if (isButtonPressed(b)) { //pressed

        if (!buttonPressedBefore) buttonPressedTime = now;
        buttonPressedBefore = true;

        if (now - buttonPressedTime > 600) { //long press
            //TODO: handleButton() handles button 0 without preset in a different way for double click
            //so we need to override with same behaviour
            //DEBUG_PRINTLN(F("4LD action."));
            //if (!buttonLongPressed) longPressAction(0);
            buttonLongPressed = true;
            return false;
        }

    } else if (!isButtonPressed(b) && buttonPressedBefore) { //released

        long dur = now - buttonPressedTime;
        if (dur < 50) {
            buttonPressedBefore = false;
            return true;
        } //too short "press", debounce

        bool doublePress = buttonWaitTime; //did we have short press before?
        buttonWaitTime = 0;

        if (!buttonLongPressed) { //short press
            // if this is second release within 350ms it is a double press (buttonWaitTime!=0)
            //TODO: handleButton() handles button 0 without preset in a different way for double click
            if (doublePress) {
                display.networkOverlay(PSTR("NETWORK INFO"), 7000);
                handled = true;
            } else {
                buttonWaitTime = now;
            }
        }
        buttonPressedBefore = false;
        buttonLongPressed = false;
    }
    // if 350ms elapsed since last press/release it is a short press
    if (buttonWaitTime && now - buttonWaitTime > 350 && !buttonPressedBefore) {
        buttonWaitTime = 0;
        //TODO: handleButton() handles button 0 without preset in a different way for double click
        //so we need to override with same behaviour
        //shortPressAction(0);
        //handled = false;
    }
    return handled;
}

void UsermodCinemagic::onUpdateBegin(bool init) {
    display.onUpdateBegin(init);
}

void UsermodCinemagic::appendConfigData() {
    oappend(SET_F("dd=addDropdown('4LineDisplay','type');"));
    oappend(SET_F("addOption(dd,'None',0);"));
    oappend(SET_F("addOption(dd,'SSD1306',1);"));
    oappend(SET_F("addOption(dd,'SH1106',2);"));
    oappend(SET_F("addOption(dd,'SSD1306 128x64',3);"));
    oappend(SET_F("addOption(dd,'SSD1305',4);"));
    oappend(SET_F("addOption(dd,'SSD1305 128x64',5);"));
    oappend(SET_F("addOption(dd,'SSD1306 SPI',6);"));
    oappend(SET_F("addOption(dd,'SSD1306 SPI 128x64',7);"));
    oappend(SET_F("addOption(dd,'SSD1309 SPI 128x64',8);"));
    oappend(SET_F("addInfo('4LineDisplay:type',1,'<br><i class=\"warn\">Change may require reboot</i>','');"));
    oappend(SET_F("addInfo('4LineDisplay:pin[]',0,'','SPI CS');"));
    oappend(SET_F("addInfo('4LineDisplay:pin[]',1,'','SPI DC');"));
    oappend(SET_F("addInfo('4LineDisplay:pin[]',2,'','SPI RST');"));
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

    top["type"] = display.type;
    JsonArray io_pin = top.createNestedArray("pin");
    for (int i = 0; i < 3; i++) io_pin.add(display.ioPin[i]);
    top[FPSTR(display._flip)] = (bool) display.flip;
    top[FPSTR(display._contrast)] = display.contrast;
    top[FPSTR(display._contrastFix)] = (bool) display.contrastFix;
    top[FPSTR(display._screenTimeOut)] = display.screenTimeout / 1000;
    top[FPSTR(display._sleepMode)] = (bool) display.sleepMode;
    top[FPSTR(display._clockMode)] = (bool) display.clockMode;
    top[FPSTR(display._showSeconds)] = (bool) display.showSeconds;
    top[FPSTR(display._busClkFrequency)] = display.ioFrequency / 1000;
    DEBUG_PRINTLN(F("4 Line Display config saved."));
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
    DisplayType newType = display.type;
    int8_t oldPin[3];
    for (byte i = 0; i < 3; i++) oldPin[i] = display.ioPin[i];

    JsonObject top = root[FPSTR(_name)];
    if (top.isNull()) {
        DEBUG_PRINT(FPSTR(_name));
        DEBUG_PRINTLN(F(": No config found. (Using defaults.)"));
        return false;
    }

    enabled = top[FPSTR(_enabled)] | enabled;
    newType = top["type"] | newType;
    for (byte i = 0; i < 3; i++) display.ioPin[i] = top["pin"][i] | display.ioPin[i];
    display.flip = top[FPSTR(display._flip)] | display.flip;
    display.contrast = top[FPSTR(display._contrast)] | display.contrast;
    display.screenTimeout = (top[FPSTR(display._screenTimeOut)] | display.screenTimeout / 1000) * 1000;
    display.sleepMode = top[FPSTR(display._sleepMode)] | display.sleepMode;
    display.clockMode = top[FPSTR(display._clockMode)] | display.clockMode;
    display.showSeconds = top[FPSTR(display._showSeconds)] | display.showSeconds;
    display.contrastFix = top[FPSTR(display._contrastFix)] | display.contrastFix;
    if (newType == SSD1306_SPI || newType == SSD1306_SPI64)
        display.ioFrequency = min(20000, max(500, (int) (top[FPSTR(display._busClkFrequency)] | display.ioFrequency / 1000))) *
                      1000;  // limit frequency
    else
        display.ioFrequency = min(3400, max(100, (int) (top[FPSTR(display._busClkFrequency)] | display.ioFrequency / 1000))) *
                      1000;  // limit frequency

    DEBUG_PRINT(FPSTR(_name));
    if (!mInited) {
        // first run: reading from cfg.json
        display.type = newType;
        DEBUG_PRINTLN(F(" config loaded."));
    } else {
        DEBUG_PRINTLN(F(" config (re)loaded."));
        display.onConfigUpdated(newType, oldPin);
    }
    // use "return !top["newestParameter"].isNull();" when updating Usermod with new features
    return !top[FPSTR(display._contrastFix)].isNull();
}
