#pragma once

#include "wled.h"
#include "pin_manager.h"
#include <INA226.h>
#ifdef USERMOD_CINEMAGIC_OLED
#include "cinemagic_display.h"
#endif

extern "C" uint8_t temprature_sens_read();

#ifdef USERMOD_CINEMAGIC_WITH_I2C_SCANNER
#include "I2CScanner.h"
I2CScanner scanner;
#endif

// Power measurement
const uint8_t USERMODE_CINEMAGIC_INA_ADDR = 0x40;
#define USERMODE_CINEMAGIC_MAX_READINGS 5
#ifndef USERMOD_CINEMAGIC_PWR_MEASURE_INTERVAL
#define USERMOD_CINEMAGIC_PWR_MEASURE_INTERVAL 300
#endif

#ifndef USERMOD_CINEMAGIC_PWR_MEMORY_UPDATE_INTERVAL
#define USERMOD_CINEMAGIC_PWR_MEMORY_UPDATE_INTERVAL 10 * 1000 // 10 seconds
#endif

#ifndef USERMOD_CINEMAGIC_BATT_CAPACITY
#define USERMOD_CINEMAGIC_BATT_CAPACITY 3000 // mA
#endif

#ifndef USERMOD_CINEMAGIC_BATT_CELLS
#define USERMOD_CINEMAGIC_BATT_CELLS 1
#endif

#ifndef USERMOD_CINEMAGIC_TEMP_LEDPANEL_PIN
#define USERMOD_CINEMAGIC_TEMP_LEDPANEL_PIN -1
#endif

#ifndef USERMOD_CINEMAGIC_TEMP_BOARD_PIN
#define USERMOD_CINEMAGIC_TEMP_BOARD_PIN -1
#endif

#ifndef USERMOD_CINEMAGIC_BATT_MAX_VOLTAGE
#define USERMOD_CINEMAGIC_BATT_MAX_VOLTAGE 4.19
#endif

#ifndef USERMOD_CINEMAGIC_BATT_MIN_VOLTAGE
#define USERMOD_CINEMAGIC_BATT_MIN_VOLTAGE 3.0
#endif

class UsermodCinemagic : public Usermod {
private:
#ifdef USERMOD_CINEMAGIC_OLED
    CinemagicDisplay display;
#endif
    bool enabled{true};
    bool mInited{false};

    // string that are used multiple time (this will save some flash memory)
    static const char _name[];
    static const char _enabled[];
    static const char _capacity[];
    static const char _cycle[];

    // current power and voltage measurement
    INA226 *ina{nullptr};
    unsigned long lastPwrUpdate{0};
    unsigned long lastMemoryPwrUpdate{0};
    uint8_t mPwrIndex = 0;
    int32_t mVoltageBuffer[USERMODE_CINEMAGIC_MAX_READINGS] = {0};
    int32_t mCurrentBuffer[USERMODE_CINEMAGIC_MAX_READINGS] = {0};
    int64_t mVoltageSum{0};
    int64_t mCurrentSum{0};
    int32_t mVoltage{0};
    int32_t mCurrent{0};
    unsigned long mCycle{0};
    uint16_t mCurrentCapacity{0};
    uint16_t mBatteryFullCapacity{USERMOD_CINEMAGIC_BATT_CAPACITY};
    uint8_t mBatteryCells{USERMOD_CINEMAGIC_BATT_CELLS};

    // temperatures
    int16_t mTempCPU{0};
    int16_t mTemp1{0};
    int16_t mTemp2{0};
    unsigned long lastTempUpdate{0};
    unsigned long lastTempBriAdjust{0};
    bool mBriReducedByTemp{false};
    bool mCriticalTemp{false};
    uint8_t mCriticalOrigTemp{false};

    void initINA() {
        // Initialize buffers
        for (int i = 0; i < USERMODE_CINEMAGIC_MAX_READINGS; i++) {
            mVoltageBuffer[i] = 0;
            mCurrentBuffer[i] = 0;
        }

        // init ina226
        // Default INA226 address is 0x40
        ina = new INA226(Wire);
        if (!ina->begin(0x40)) {
            DEBUG_PRINTLN("INA226 Connection error");
        }
        ina->configure(INA226_AVERAGES_1,
                       INA226_BUS_CONV_TIME_1100US,
                       INA226_SHUNT_CONV_TIME_1100US,
                       INA226_MODE_SHUNT_BUS_CONT);
        ina->calibrate(0.02, 8);
    }

    void initTemperature(){
        analogReadResolution(12); // Set the resolution to 12-bit
        if (USERMOD_CINEMAGIC_TEMP_LEDPANEL_PIN >= 0 && pinManager.allocatePin(USERMOD_CINEMAGIC_TEMP_LEDPANEL_PIN, true, PinOwner::UM_CINEMAGIC)) {
            pinMode(USERMOD_CINEMAGIC_TEMP_LEDPANEL_PIN, INPUT);
        }
        if (USERMOD_CINEMAGIC_TEMP_BOARD_PIN >= 0 && pinManager.allocatePin(USERMOD_CINEMAGIC_TEMP_BOARD_PIN, true, PinOwner::UM_CINEMAGIC)) {
            pinMode(USERMOD_CINEMAGIC_TEMP_BOARD_PIN, INPUT);
        }
    }

    void measurePower() {
        if (millis() - lastPwrUpdate < USERMOD_CINEMAGIC_PWR_MEASURE_INTERVAL)
            return;
        lastPwrUpdate = millis();

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
        mVoltage = mVoltageSum / USERMODE_CINEMAGIC_MAX_READINGS;
        mCurrent = mCurrentSum / USERMODE_CINEMAGIC_MAX_READINGS;

        // Move to the next index in a circular manner
        mPwrIndex = (mPwrIndex + 1) % USERMODE_CINEMAGIC_MAX_READINGS;



//        mVoltage = ina->readBusVoltage();
//        mCurrent = ina->readShuntCurrent();
//        const float elapsed = (millis() - lastPwrUpdate) / 1000.f; // seconds
//        const float consumed = elapsed / 3600.f * mVoltage * mCurrent;
//        mCurrentCapacity -= consumed;
//        mCycle += consumed;
//        lastPwrUpdate = millis();
//
//        if (mBatteryCells * USERMOD_CINEMAGIC_BATT_MAX_VOLTAGE <= mVoltage) {
//            mCurrentCapacity = mBatteryFullCapacity;
//        }
//        if (mBatteryCells * USERMOD_CINEMAGIC_BATT_MIN_VOLTAGE >= mVoltage) {
//            mCurrentCapacity = 0;
//        }

#ifdef USERMOD_CINEMAGIC_MAX_SAFE_CURRENT
        static int16_t briMultiplierInt = briMultiplier;  // Retain as integer with one decimal place
        bool updateRequired = false;  // Track if update is needed

        if (mCurrent > USERMOD_CINEMAGIC_MAX_SAFE_CURRENT / 10 + 5) {
            briMultiplierInt = max((briMultiplierInt * 98) / 100, 10);  // Ensure it doesn't go below 10.0 (represented as 100)
            updateRequired = true;
            mBriReducedByTemp = false;
        } else if (mCurrent < USERMOD_CINEMAGIC_MAX_SAFE_CURRENT / 10 - 5 && briMultiplierInt < 1000 && !mBriReducedByTemp) {
            briMultiplierInt = min((briMultiplierInt * 102) / 100, 100);  // Ensure it doesn't exceed 100.0 (represented as 1000)
            updateRequired = true;
        }

        if (updateRequired) {
            updateBrightnessCap(briMultiplierInt);
        }
#endif
        // update battery capacity in ROM
//        if (millis() - lastMemoryPwrUpdate > USERMOD_CINEMAGIC_PWR_MEMORY_UPDATE_INTERVAL) {
//            saveToConfig();
//            lastMemoryPwrUpdate = millis();
//            DEBUG_PRINTF("Cinemagic: Power parameters saved in the memory!!!\n");
//        }

#ifdef USERMOD_CINEMAGIC_OLED
        display.updateBatteryInfo(mVoltage, mCurrent, mCurrentCapacity, mBatteryFullCapacity);
#endif
//        DEBUG_PRINTF(
//                "Cinemagic: Voltage %.3fV, Current %.3fC, Power %.3fW, Capacity: %.3fW, Percentage: %.2f, Temp1: %.2f\n",
//                mVoltage, mCurrent, mVoltage * mCurrent, mCurrentCapacity,
//                mCurrentCapacity / mBatteryFullCapacity * 100, mTemp1);
    }

    void measureTemperature(){
        if (millis() - lastTempUpdate > USERMOD_CINEMAGIC_PWR_MEASURE_INTERVAL) {
            lastTempUpdate = millis();

            const uint32_t R25 = 10000;        // Resistance at 25°C (10k ohms)
            const uint32_t B = 3950;           // B constant
            const uint32_t ADC_MAX = 4095;     // 12-bit ADC maximum value

            uint16_t analogValue;
            float resistance, tempK, temperatureC;
            int16_t temperatureC_x100; // Temperature in 0.01°C units

#ifdef ARDUINO_ARCH_ESP32
            // CPU Temperature in 0.01°C units
            mTempCPU = static_cast<int16_t>(((temprature_sens_read() - 32) * 1000) / 18);
#endif

            if (USERMOD_CINEMAGIC_TEMP_LEDPANEL_PIN > -1) {
                analogValue = analogRead(USERMOD_CINEMAGIC_TEMP_LEDPANEL_PIN);
                if (analogValue == 0) analogValue = 1; // Prevent division by zero
                resistance = 10000.0f * (ADC_MAX / analogValue - 1.0f);
                tempK = 1.0f / ((logf(resistance / R25) / B) + (1.0f / 298.15f));
                temperatureC = tempK - 273.15f;
                temperatureC_x100 = static_cast<int16_t>(temperatureC * 100.0f);
                mTemp1 = -temperatureC_x100 + 4700; // mTemp1 in 0.01°C units
            }

            if (USERMOD_CINEMAGIC_TEMP_BOARD_PIN > -1) {
                analogValue = analogRead(USERMOD_CINEMAGIC_TEMP_BOARD_PIN);
                if (analogValue == 0) analogValue = 1; // Prevent division by zero
                resistance = 10000.0f * (ADC_MAX / analogValue - 1.0f);
                tempK = 1.0f / ((logf(resistance / R25) / B) + (1.0f / 298.15f));
                temperatureC = tempK - 273.15f;
                temperatureC_x100 = static_cast<int16_t>(temperatureC * 100.0f);
                mTemp2 = -temperatureC_x100 + 4700; // mTemp2 in 0.01°C units
            }

#ifdef USERMOD_CINEMAGIC_CRITICAL_TEMP
            if (mTemp2 > USERMOD_CINEMAGIC_CRITICAL_TEMP * 100){
                mCriticalOrigTemp = briMultiplier;
                mBriReducedByTemp = true;
                mCriticalTemp = true;
                updateBrightnessCap(1);
            } else if (mCriticalTemp && mTemp2 < USERMOD_CINEMAGIC_CRITICAL_TEMP * 100 - 500){
                mCriticalTemp = false;
                updateBrightnessCap(briMultiplier);
            }
#endif
#ifdef USERMOD_CINEMAGIC_MAX_SAFE_TEMP
            if (millis() - lastTempBriAdjust > 60000 && !mCriticalTemp){
                static int16_t briMultiplierInt = briMultiplier;  // Retain as integer with one decimal place
                bool updateRequired = false;  // Track if update is needed
                int16_t maxTemp = max(mTemp1, mTemp2);

                if (maxTemp > USERMOD_CINEMAGIC_MAX_SAFE_TEMP * 100 + 100) {
                    briMultiplierInt = max((briMultiplierInt * 95) / 100, 10);  // Ensure it doesn't go below 10.0 (represented as 100)
                    updateRequired = true;
                    mBriReducedByTemp = true;
                } else if (maxTemp < USERMOD_CINEMAGIC_MAX_SAFE_CURRENT * 100 - 100 && briMultiplierInt < 1000 && mBriReducedByTemp) {
                    briMultiplierInt = min((briMultiplierInt * 105) / 100, 100);  // Ensure it doesn't exceed 100.0 (represented as 1000)
                    updateRequired = true;
                }

                if (updateRequired) {
                    updateBrightnessCap(briMultiplierInt);
                }
                lastTempBriAdjust = millis();
            }
#endif

#ifdef USERMOD_CINEMAGIC_OLED
            if (USERMOD_CINEMAGIC_TEMP_LEDPANEL_PIN > -1 || USERMOD_CINEMAGIC_TEMP_BOARD_PIN > -1){
                display.updateTemperatureInfo(mTemp1, mTemp2);
            }
#endif
        }
    }

    void updateBrightnessCap(int16_t brightness){
        briMultiplier = static_cast<byte>(brightness);  // Convert back to integer percentage (0-100)
        strip.setBrightness((bri * briMultiplier) / 100);
        updateInterfaces(CALL_MODE_DIRECT_CHANGE);  // Ensure the UI updates
        strip.show();
    }

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

        initINA();

#ifdef USERMOD_CINEMAGIC_WITH_I2C_SCANNER
        scanner.Init();
        delay(5000);
        scanner.Scan();
#endif

        // temperature
        initTemperature();

        mInited = true;
    }


    /*
     * connected() is called every time the WiFi is (re)connected
     * Use it to initialize network interfaces
     */
    void connected() {
#ifdef USERMOD_CINEMAGIC_OLED
        display.knownSsid = WiFi.SSID();       //apActive ? apSSID : WiFi.SSID(); //apActive ? WiFi.softAPSSID() :
        display.knownIp = Network.localIP(); //apActive ? IPAddress(4, 3, 2, 1) : Network.localIP();
        display.networkOverlay(PSTR("NETWORK INFO"), 7000);
#endif
    }

    void loop() {
        // if usermod is disabled or called during strip updating just exit
        // NOTE: on very long strips strip.isUpdating() may always return true so update accordingly
        if (!enabled || strip.isUpdating()) return;

//        if (millis() - lastupdate > 2000) {
//            if (!mBYPassSWState) {
//                turnOnRGB();
//            } else {
//                turnOffRGB();
//            }
//            lastupdate = millis();
//        }

        // power measurement
        measurePower();

        // temperature
        measureTemperature();

        // display update
#ifdef USERMOD_CINEMAGIC_OLED
        display.loop();
#endif
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

        // Add battery voltage to the JSON info
        JsonArray batteryVoltageArr = user.createNestedArray(F("Battery Voltage"));
        batteryVoltageArr.add((float)mVoltage / 100.0f); // Add the voltage value
        batteryVoltageArr.add(F("V")); // Add the unit

        // Add current usage to the JSON info
        JsonArray currentUsageArr = user.createNestedArray(F("Current Usage"));
        currentUsageArr.add((float)mCurrent / 100.0f); // Add the current value
        currentUsageArr.add(F("A")); // Add the unit

        // Add temp 1 to the JSON info
        JsonArray temp1Arr = user.createNestedArray(F("CPU Temp"));
        temp1Arr.add((float)mTempCPU / 100.0f);
        temp1Arr.add(F("C"));

        // Add temp 1 to the JSON info
        if (USERMOD_CINEMAGIC_TEMP_LEDPANEL_PIN > -1) {
            JsonArray temp1Arr = user.createNestedArray(F("LED Panel Temp"));
            temp1Arr.add((float)mTemp1 / 100.0f);
            temp1Arr.add(F("C"));
        }

        // Add temp 2 to the JSON info
        if (USERMOD_CINEMAGIC_TEMP_BOARD_PIN > -1) {
            JsonArray temp2Arr = user.createNestedArray(F("Mainboard Temp"));
            temp2Arr.add((float)mTemp2 / 100.0f);
            temp2Arr.add(F("C"));
        }

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
const char UsermodCinemagic::_capacity[]         PROGMEM = "capacity";
const char UsermodCinemagic::_cycle[]         PROGMEM = "cycle";

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
#ifdef USERMOD_CINEMAGIC_OLED
                display.networkOverlay(PSTR("NETWORK INFO"), 7000);
#endif
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
#ifdef USERMOD_CINEMAGIC_OLED
    display.onUpdateBegin(init);
#endif
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

    // battery configurations
    top[FPSTR(_capacity)] = mCurrentCapacity;
    top[FPSTR(_cycle)] = mCycle;

#ifdef USERMOD_CINEMAGIC_OLED
    JsonArray io_pin = top.createNestedArray("pin");
    for (int i = 0; i < 3; i++) io_pin.add(display.ioPin[i]);
    top["type"] = display.type;
    DEBUG_PRINTLN(F("4 Line Display config saved."));
    top[FPSTR(display._flip)] = (bool) display.flip;
    top[FPSTR(display._contrast)] = display.contrast;
    top[FPSTR(display._contrastFix)] = (bool) display.contrastFix;
    top[FPSTR(display._screenTimeOut)] = display.screenTimeout / 1000;
    top[FPSTR(display._sleepMode)] = (bool) display.sleepMode;
    top[FPSTR(display._clockMode)] = (bool) display.clockMode;
    top[FPSTR(display._showSeconds)] = (bool) display.showSeconds;
    top[FPSTR(display._busClkFrequency)] = display.ioFrequency / 1000;
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
#ifdef USERMOD_CINEMAGIC_OLED
    DisplayType newType = display.type;
    int8_t oldPin[3];
    for (byte i = 0; i < 3; i++) oldPin[i] = display.ioPin[i];
#endif
    JsonObject top = root[FPSTR(_name)];
    if (top.isNull()) {
        DEBUG_PRINT(FPSTR(_name));
        DEBUG_PRINTLN(F(": No config found. (Using defaults.)"));
        return false;
    }

    enabled = top[FPSTR(_enabled)] | enabled;

    // read battery configurations
    mCurrentCapacity = top[FPSTR(_capacity)] | 0.0;
    mCycle = top[FPSTR(_cycle)] | 0.0;

    // read display configurations
#ifdef USERMOD_CINEMAGIC_OLED
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
        display.ioFrequency =
                min(20000, max(500, (int) (top[FPSTR(display._busClkFrequency)] | display.ioFrequency / 1000))) *
                1000;  // limit frequency
    else
        display.ioFrequency =
                min(3400, max(100, (int) (top[FPSTR(display._busClkFrequency)] | display.ioFrequency / 1000))) *
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
#endif
    return true;
}

void UsermodCinemagic::saveToConfig() {
    // Trigger WLED to save the configuration including your variable
    serializeConfig();
}
