#pragma once

#include "wled.h"
#include "pin_manager.h"
#include "cinemagic_display.h"
#include <INA226.h>

#ifdef USERMOD_CINEMAGIC_WITH_BQ2589
#include "BQ2589/bq2589x.h"
#endif

#ifdef USERMOD_CINEMAGIC_WITH_PD
#include "STUSB4500/STUSB4500.h"
#define USBPD_RST_PIN  -1
#define USBPD_ATCH_PIN -1
#define USBPD_ALRT_PIN -1

#define PDO_STR_LEN    48
#endif

#ifdef USERMOD_CINEMAGIC_WITH_I2C_SCANNER
#include "I2CScanner.h"
I2CScanner scanner;
#endif

// Power measurement
#ifndef USERMOD_CINEMAGIC_PWR_MEASURE_INTERVAL
#define USERMOD_CINEMAGIC_PWR_MEASURE_INTERVAL 500
#endif

#ifndef USERMOD_CINEMAGIC_PWR_MEMORY_UPDATE_INTERVAL
#define USERMOD_CINEMAGIC_PWR_MEMORY_UPDATE_INTERVAL 10 * 1000 // 10 seconds
#endif

#ifndef USERMOD_CINEMAGIC_BATT_CAPACITY
#define USERMOD_CINEMAGIC_BATT_CAPACITY 24
#endif

#ifndef USERMOD_CINEMAGIC_BATT_CELLS
#define USERMOD_CINEMAGIC_BATT_CELLS 1
#endif

#define USERMOD_CINEMAGIC_CHARGE_DIS_PIN 13
#define USERMOD_CINEMAGIC_TEMP_LEDPANEL_PIN 32


#ifndef USERMOD_CINEMAGIC_BATT_MAX_VOLTAGE
#define USERMOD_CINEMAGIC_BATT_MAX_VOLTAGE 4.19
#endif

#ifndef USERMOD_CINEMAGIC_BATT_MIN_VOLTAGE
#define USERMOD_CINEMAGIC_BATT_MIN_VOLTAGE 2.9
#endif

// STUSB4500
#ifdef USERMOD_CINEMAGIC_WITH_PD
#define STUSB4500_ADDR 0x28  // The I2C address of the STUSB4500
#define NVM_COMMAND 0x1A  // Register for NVM command
#define NVM_UNLOCK 0x95  // Command to unlock NVM
#define NVM_LOCK 0x97  // Command to lock NVM

#define PDO1_REGISTER 0x85  // Base register for PDO1
#define PDO1_VOLTAGE 0x23  // Offset for PDO1 voltage
#define PDO1_CURRENT 0x25  // Offset for PDO1 current

#define EXPECTED_VOLTAGE 0x8C  // Expected value for 15V
#define EXPECTED_CURRENT 0x12C  // Expected value for 3A
#define STATUS_VOLTAGE 0x54  // Register for current voltage status
#define STATUS_CURRENT 0x56  // Register for current current status

#define STATUS_REGISTER 0x28  // Status register (example, verify from datasheet)
#define VOLTAGE_OFFSET 0x85  // Actual register offset for voltage
#define CURRENT_OFFSET 0x87  // Actual register offset for current
#endif

class UsermodCinemagic : public Usermod {
private:
    CinemagicDisplay display;
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
    float mVoltage{0};
    float mCurrent{0};
    float mCycle{0};
    float mCurrentCapacity{0};
    uint8_t mBatteryFullCapacity{USERMOD_CINEMAGIC_BATT_CAPACITY};
    uint8_t mBatteryCells{USERMOD_CINEMAGIC_BATT_CELLS};

    float mTemp1{0};
    unsigned long lastTempUpdate{0};

#ifdef USERMOD_CINEMAGIC_WITH_BQ2589
    bq2589x *mBQ{nullptr};
    unsigned long lastBQUpdate{0};
#endif


#ifdef USERMOD_CINEMAGIC_WITH_PD
    unsigned long pdlastupdate{0};
    bool mPDDeviceFound{false};
    STUSB4500 *usbpd{nullptr};
#endif

    void initINA() {
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

    void measurePower() {
        if (millis() - lastPwrUpdate < USERMOD_CINEMAGIC_PWR_MEASURE_INTERVAL)
            return;

        if (!ina) {
            DEBUG_PRINTLN("Cinemagic: Error in measuring power. INA is not initiated!");
            return;
        }
        mVoltage = ina->readBusVoltage();
        mCurrent = ina->readShuntCurrent();
        const float elapsed = (millis() - lastPwrUpdate) / 1000.f; // seconds
        const float consumed = elapsed / 3600.f * mVoltage * mCurrent;
        mCurrentCapacity -= consumed;
        mCycle += consumed;
        lastPwrUpdate = millis();

        if (mBatteryCells * USERMOD_CINEMAGIC_BATT_MAX_VOLTAGE <= mVoltage) {
            mCurrentCapacity = mBatteryFullCapacity;
        }
        if (mBatteryCells * USERMOD_CINEMAGIC_BATT_MIN_VOLTAGE >= mVoltage) {
            mCurrentCapacity = 0;
        }

        // update battery capacity in ROM
//        if (millis() - lastMemoryPwrUpdate > USERMOD_CINEMAGIC_PWR_MEMORY_UPDATE_INTERVAL) {
//            saveToConfig();
//            lastMemoryPwrUpdate = millis();
//            DEBUG_PRINTF("Cinemagic: Power parameters saved in the memory!!!\n");
//        }

        display.updateBatteryInfo(mVoltage, mCurrent, mCurrentCapacity, mBatteryFullCapacity);
        DEBUG_PRINTF(
                "Cinemagic: Voltage %.3fV, Current %.3fC, Power %.3fW, Capacity: %.3fW, Percentage: %.2f, Temp1: %.2f\n",
                mVoltage, mCurrent, mVoltage * mCurrent, mCurrentCapacity,
                mCurrentCapacity / mBatteryFullCapacity * 100, mTemp1);
    }

    void BQInit() {
#ifdef USERMOD_CINEMAGIC_WITH_BQ2589
        mBQ = new bq2589x();
        mBQ->begin(0x6A, &Wire);

        // Disable HiZ mode
        if (mBQ->exit_hiz_mode() != BQ2589X_OK) {
            Serial.println("Failed to exit HiZ mode");
        }

        // Set input current limit to 3A (130 ohm resistor on ILIM)
        if (mBQ->set_input_current_limit(5000) != BQ2589X_OK) {
            Serial.println("Failed to set input current limit");
        }

        // Set charge current to 2.5A
        if (mBQ->set_charge_current(5000) != BQ2589X_OK) {
            Serial.println("Failed to set charge current");
        }
        if (mBQ->set_prechg_current(400) != BQ2589X_OK) {
            Serial.println("Failed to set pre-charge current");
        }

        // Set minimum system voltage to 3.5V
        if (mBQ->set_input_volt_limit(3500) != BQ2589X_OK) {
            Serial.println("Failed to set input voltage limit");
        }

        // Start ADC conversion
        if (mBQ->adc_start(false) != BQ2589X_OK) {
            Serial.println("Failed to start ADC");
        }

        // Enable auto DPDM detection
        if (mBQ->enable_auto_dpdm(true) != BQ2589X_OK) {
            Serial.println("Failed to enable auto DPDM");
        }

        if (mBQ->disable_watchdog_timer() != BQ2589X_OK) {
            Serial.println("Failed to eDisable watchdog");
        }

        // Enable charger
        if (mBQ->enable_charger() != BQ2589X_OK) {
            Serial.println("Failed to enable charger");
        }

        Serial.println("BQ25895 initialization complete");
#endif
    }

    void BQLoop() {
#ifdef USERMOD_CINEMAGIC_WITH_BQ2589
        if (millis() - lastBQUpdate < 1000)
            return;
        lastBQUpdate = millis();

        if (!mBQ) {
            DEBUG_PRINTLN("Cinemagic: Error in connecting to bq bms chip!");
            return;
        }

//        mBQ->reset_watchdog_timer();

        // Debugging read operations
        int batteryVolt = mBQ->adc_read_battery_volt();
        int sysVolt = mBQ->adc_read_sys_volt();
        int busVolt = mBQ->adc_read_vbus_volt();
        int temperature = mBQ->adc_read_temperature();
        int chargeCurrent = mBQ->adc_read_charge_current();
        int idpmLimit = mBQ->read_idpm_limit();
        int chargingStatus = mBQ->get_charging_status();

        DEBUG_PRINTF("Cinemagic: BQ BMS, BAT Voltage %dmV, SYS Voltage: %dmV, BUS Voltage: %dmV, Temp: %d°C, Charge Current: %dmA, IDMP Limit: %dmA, Status: %d\n",
                     batteryVolt, sysVolt, busVolt, temperature, chargeCurrent, idpmLimit, chargingStatus);
//        DEBUG_PRINTF(
//                "Cinemagic: BQ BMS, BAT Voltage %.3fV, SYS Voltage: %.3f, BUS Voltage: %.3f, Temp: %.1f, Charge Current: %.3fA, IDMP Limit: %0.3fA, Charge Status: %s\n",
//                mBQ->adc_read_battery_volt(), mBQ->adc_read_sys_volt(), mBQ->adc_read_vbus_volt(),
//                mBQ->adc_read_temperature(), mBQ->adc_read_charge_current(), mBQ->read_idpm_limit(),
//                (mBQ->get_charging_status() == 0 ? "Not Charging" : (mBQ->get_charging_status() == 1 ? "Pre Charging" : (mBQ->get_charging_status() == 2 ? "Fast Charging" : "Charge Terminations Done")))
//        );
#endif
    }

#ifdef USERMOD_CINEMAGIC_WITH_PD

    uint8_t PDReadRegister(uint8_t reg) {
        if (!mPDDeviceFound) {
            return 0;
        }
//        do {
            Wire.beginTransmission(STUSB4500_ADDR);
            Wire.write(reg);
        if (Wire.endTransmission() != 0){
        DEBUG_PRINTLN("Cinemagic: PDReadRegister() error in write");
        }

//        do {
            Wire.requestFrom(STUSB4500_ADDR, 1);
//        } while (Wire.available() != 0);
//        return Wire.read();
        if (Wire.available()) {
            return Wire.read();
        }
        DEBUG_PRINTLN("Cinemagic: PDReadRegister() error");
        return 0;  // Return 0 if read fails
    }

    // Function to read a 16-bit register from STUSB4500
    uint16_t PDReadRegister16(uint8_t reg) {
        if (!mPDDeviceFound) {
            return 0;
        }
        uint16_t value = 0;

//        do {
            delay(10);
            Wire.beginTransmission(STUSB4500_ADDR);
            Wire.write(reg);
//        } while (Wire.endTransmission() != 0);
        if (Wire.endTransmission() != 0) {
            DEBUG_PRINTLN("Cinemagic: PDReadRegister16() write error");
            return 0;
        }

        delay(10);  // Small delay to ensure device is ready

//        do {
            Wire.requestFrom(STUSB4500_ADDR, 2);  // Request 2 bytes (16 bits)
//        } while (Wire.available() >= 2);
//        uint8_t lowerByte = Wire.read();  // Read the lower byte
//        uint8_t upperByte = Wire.read();  // Read the upper byte
//        value = (upperByte << 8) | lowerByte;  // Combine the bytes
        if (Wire.available() >= 2) {
            uint8_t lowerByte = Wire.read();  // Read the lower byte
            uint8_t upperByte = Wire.read();  // Read the upper byte
            value = (upperByte << 8) | lowerByte;  // Combine the bytes
        } else {
            DEBUG_PRINTLN("Cinemagic: PDReadRegister16() read error");
        }

        return value;
    }

// Function to configure the STUSB4500
    void PDConfigure() {
        if (!mPDDeviceFound) {
            return;
        }
        // Unlock NVM
//        do {
            delay(10);
            Wire.beginTransmission(STUSB4500_ADDR);
            Wire.write(NVM_COMMAND);
            Wire.write(NVM_UNLOCK);
//        } while (Wire.endTransmission() != 0);
//        DEBUG_PRINTLN("Cinemagic: NVM unlock DONE");
        if (Wire.endTransmission() != 0) {
            DEBUG_PRINTLN("Cinemagic: NVM unlock error");
            return;
        }

        delay(10);  // Small delay to ensure device is ready

        // Set PDO1 voltage to 15V
//        do {
            delay(10);
            Wire.beginTransmission(STUSB4500_ADDR);
            Wire.write(PDO1_REGISTER + PDO1_VOLTAGE);
            Wire.write(EXPECTED_VOLTAGE);  // Value for 15V
//        } while (Wire.endTransmission() != 0);
//        DEBUG_PRINTLN("Cinemagic: PDO1 voltage set DONE");
        if (Wire.endTransmission() != 0) {
            DEBUG_PRINTLN("Cinemagic: PDO1 voltage set error");
            return;
        }

        delay(10);  // Small delay to ensure device is ready

        // Set PDO1 current to max value (3A)
//        do {
            delay(10);
            Wire.beginTransmission(STUSB4500_ADDR);
            Wire.write(PDO1_REGISTER + PDO1_CURRENT);
            Wire.write(EXPECTED_CURRENT);  // Value for 3A
//        } while (Wire.endTransmission() != 0);
//        DEBUG_PRINTLN("Cinemagic: PDO1 current set DONE");
        if (Wire.endTransmission() != 0) {
            DEBUG_PRINTLN("Cinemagic: PDO1 current set error");
            return;
        }

        delay(10);  // Small delay to ensure device is ready

        // Lock NVM to save settings
//        do {
            delay(10);
            Wire.beginTransmission(STUSB4500_ADDR);
            Wire.write(NVM_COMMAND);
            Wire.write(NVM_LOCK);
//        } while (Wire.endTransmission() != 0);
//        DEBUG_PRINTLN("Cinemagic: NVM lock DONE");
        if (Wire.endTransmission() != 0) {
            DEBUG_PRINTLN("Cinemagic: NVM lock error");
            return;
        }

        DEBUG_PRINTLN("Cinemagic: PDConfigure() DONE");
    }

#endif

    void initPD() {
#ifdef USERMOD_CINEMAGIC_WITH_PD
////        do {
////            Wire.beginTransmission(STUSB4500_ADDR);
////            delay(100);
////        } while (Wire.endTransmission() != 0);
////        mPDDeviceFound = true;
////        DEBUG_PRINTLN("Cinemagic: Found PD chip.");
//
//        Wire.beginTransmission(STUSB4500_ADDR);
//        if (Wire.endTransmission() == 0) {
//            mPDDeviceFound = true;
//            DEBUG_PRINTLN("Cinemagic: Found PD chip.");
//        } else {
//            DEBUG_PRINTLN("Cinemagic: Cannot find PD chip.");
//            return;
//        }
//
//        // Read back the current configuration
//        uint16_t currentVoltage = PDReadRegister16(STATUS_REGISTER + VOLTAGE_OFFSET);
//        DEBUG_PRINT("Cinemagic: Current Voltage Raw: ");
//        DEBUG_PRINTLN(currentVoltage);
//
//        uint16_t currentCurrent = PDReadRegister16(STATUS_REGISTER + CURRENT_OFFSET);
//        DEBUG_PRINT("Cinemagic: Current Current Raw: ");
//        DEBUG_PRINTLN(currentCurrent);
//
//        // Check if the configuration matches the expected values
//        if (currentVoltage != EXPECTED_VOLTAGE || currentCurrent != EXPECTED_CURRENT) {
//            DEBUG_PRINTLN("Cinemagic: PDConfigure() Starting ...");
//            PDConfigure();  // Configure the STUSB4500 if values are incorrect
//        } else {
//            DEBUG_PRINTLN("Cinemagic: PDConfigure() Skipped, PD already configured");
//        }
        usbpd = new STUSB4500(USBPD_RST_PIN, STUSB4500_ADDR, &Wire);
        if (usbpd->begin(USBPD_ALRT_PIN, USBPD_ATCH_PIN)) {
            DEBUG_PRINT("CINEMAGIC: STUSB4500 v");
            DEBUG_PRINTLN(usbpd->version());

            // set power to default USB (5V 1.5A) initially
//            usbpd.setPowerDefaultUSB();

// Ensure the device is ready
            if (!usbpd->ready()) {
                Serial.println("Device not ready");
                return;
            }

            // Request source capabilities from the PD source
            if (!usbpd->requestSourceCapabilities()) {
                Serial.println("Failed to request source capabilities");
                return;
            }

            // Set the desired power profile
            if (!usbpd->setPower(9000, 3000)) {
                Serial.println("Failed to set power");
                return;
            }
        } else {
            Serial.println("failed to initialize STUSB4500");
        }
#endif
    }

    void PDLoop() {
#ifdef USERMOD_CINEMAGIC_WITH_PD

        if (millis() - pdlastupdate < 1000)
            return;

        pdlastupdate = millis();

        PDO requestedPDO = usbpd->requestedPDO();
        if (requestedPDO.number > 0) {
            DEBUG_PRINTF("Cinemagic: PD Voltage %dV, Current %dC\n", requestedPDO.voltage_mV, requestedPDO.current_mA);
        } else {
            DEBUG_PRINTLN("Cinemagic: No power delivery profile active");
            Serial.println("No power delivery profile active");
        }
//        uint16_t voltageRaw = PDReadRegister16(STATUS_VOLTAGE);
//        uint16_t currentRaw = PDReadRegister16(STATUS_CURRENT);
//
//        // Convert raw values to actual voltage and current
//        float voltage = voltageRaw * 0.05;  // Voltage in volts (assuming 50mV steps)
//        float current = currentRaw * 0.01;  // Current in amps (assuming 10mA steps)
//
//        // Display the values (replace with actual display or serial print code)
//        Serial.print("Voltage: ");
//        Serial.print(voltage);
//        Serial.println(" V");
//
//        Serial.print("Current: ");
//        Serial.print(current);
//        Serial.println(" A");
//
//        DEBUG_PRINTF("Cinemagic: PD Voltage %.3fV, Current %.3fC\n", voltage, current);
#endif
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

        display.beginType2();
        initINA();
        initPD();

#ifdef USERMOD_CINEMAGIC_WITH_I2C_SCANNER
        scanner.Init();
        delay(5000);
        scanner.Scan();
#endif

        // temperature
        analogReadResolution(12); // Set the resolution to 12-bit
        if (pinManager.allocatePin(USERMOD_CINEMAGIC_TEMP_LEDPANEL_PIN, true, PinOwner::None)) {
            pinMode(USERMOD_CINEMAGIC_TEMP_LEDPANEL_PIN, INPUT);
        }

        // chargedis control
        if (pinManager.allocatePin(USERMOD_CINEMAGIC_CHARGE_DIS_PIN, true, PinOwner::BusDigital)) {
            pinMode(USERMOD_CINEMAGIC_CHARGE_DIS_PIN, OUTPUT);
            digitalWrite(USERMOD_CINEMAGIC_CHARGE_DIS_PIN, LOW);
        }

        // BQ2589 Charger controller
        BQInit();

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
//            if (!mBYPassSWState) {
//                turnOnRGB();
//            } else {
//                turnOffRGB();
//            }
//            lastupdate = millis();
//        }

        // power measurement
        measurePower();

        // PD Controller
        PDLoop();

        // BQ BMS
        BQLoop();

        // temperature
        if (millis() - lastTempUpdate > USERMOD_CINEMAGIC_PWR_MEASURE_INTERVAL) {
            lastTempUpdate = millis();
            const float R25 = 10000.0;        // Resistance at 25°C (10k ohms)
            const float B = 3950.0;           // B constant

            int analogValue = analogRead(USERMOD_CINEMAGIC_TEMP_LEDPANEL_PIN);

            if (analogValue == 0) analogValue = 1;

            float voltage = analogValue * (3.3 / 4095.0);
            float resistance = 10000.f * (3.3f / voltage - 1);
            float tempK = 1.f / ((log(resistance / R25) / B) + (1.f / 298.15)); // 298.15 K is 25°C
            float temperatureC = tempK - 273.15f; // Convert Kelvin to Celsius
            mTemp1 = -1 * temperatureC + 47;
        }

        // display update
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

        // Add battery voltage to the JSON info
        JsonArray batteryVoltageArr = user.createNestedArray(F("Battery Voltage"));
        batteryVoltageArr.add(mVoltage); // Add the voltage value
        batteryVoltageArr.add(F("V")); // Add the unit

        // Add current usage to the JSON info
        JsonArray currentUsageArr = user.createNestedArray(F("Current Usage"));
        currentUsageArr.add(mCurrent); // Add the current value
        currentUsageArr.add(F("A")); // Add the unit
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

    // battery configurations
    top[FPSTR(_capacity)] = mCurrentCapacity;
    top[FPSTR(_cycle)] = mCycle;

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

    // read battery configurations
    mCurrentCapacity = top[FPSTR(_capacity)] | 0.0;
    mCycle = top[FPSTR(_cycle)] | 0.0;

    // read display configurations
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
}

void UsermodCinemagic::saveToConfig() {
    // Trigger WLED to save the configuration including your variable
    serializeConfig();
}
