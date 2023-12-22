//
// Created by Mohsen Basiti on 2023-12-18.
// modified version of usermode_temprature.h
// temperature read creadits goes to Miles Burton for DallasTemperature library
//
#pragma once

#include "wled.h"
#include "OneWire.h"


//Pin defaults for QuinLed Dig-Uno if not overriden
#ifndef CM_TEMPERATURE_PIN
#ifdef USERMOD_CINEMAGIC_TEMPERATURE_PIN
#define CM_TEMPERATURE_PIN USERMOD_CINEMAGIC_TEMPERATURE_PIN
#else
#define CM_TEMPERATURE_PIN 5
#endif
#endif

// the frequency to check temperature, 1 minute
#ifndef USERMOD_DALLASTEMPERATURE_MEASUREMENT_INTERVAL
#define USERMOD_DALLASTEMPERATURE_MEASUREMENT_INTERVAL 2000
#endif


// Model IDs
#define DS18S20MODEL 0x10  // also DS1820
#define DS18B20MODEL 0x28  // also MAX31820
#define DS1822MODEL  0x22
#define DS1825MODEL  0x3B  // also MAX31850
#define DS28EA00MODEL 0x42
#define TEMP_DEVICE_DISCONNECTED_RAW -7040

class UsermodCinemagicTemperature : public Usermod {

private:
    typedef uint8_t DeviceAddress[8];
    typedef uint8_t ScratchPad[9];

    bool initDone = false;
    OneWire *oneWire;
    // GPIO pin used for sensor (with a default compile-time fallback)
    int8_t temperaturePin = CM_TEMPERATURE_PIN;
    // measurement unit (true==°C, false==°F)
    bool degC = true;
    // using parasite power on the sensor
    bool parasite = false;
    int8_t parasitePin = -1;
    // how often do we read from sensor?
    unsigned long readingInterval = USERMOD_CINEMAGIC_TEMPERATURE_INTERVAL;
    // set last reading as "40 sec before boot", so first reading is taken after 20 sec
    unsigned long lastMeasurement = 0;
    // last time requestTemperatures was called
    // used to determine when we can read the sensors temperature
    // we have to wait at least 93.75 ms after requestTemperatures() is called
    unsigned long lastTemperaturesRequest;

    // indicates requestTemperatures has been called but the sensor measurement is not complete
    bool waitingForConversion = false;

    DeviceAddress *sensors;
    uint8_t sensorCount = 0;
    int32_t *temperatures;

    bool enabled = true;

    bool HApublished = false;

    // strings to reduce flash memory usage (used more than twice)
    static const char _name[];
    static const char _enabled[];
    static const char _readInterval[];
    static const char _parasite[];
    static const char _parasitePin[];

    void requestTemperatures();
    void readAllTemperatures();
    int32_t getRawTemperature(const uint8_t* deviceAddress);
    bool findSensors();
    int32_t calculateTemperature(const uint8_t* deviceAddress, uint8_t* scratchPad);
    float rawToCelsius(int32_t raw);
    float rawToFahrenheit(int32_t raw);
    bool isSensorConnected(const uint8_t* deviceAddress);
    bool isSensorConnected(const uint8_t* deviceAddress, uint8_t* scratchPad);
    bool isAllZeros(const uint8_t * const scratchPad, const size_t length = 9);
    bool readScratchPad(const uint8_t* deviceAddress, uint8_t* scratchPad);
#ifndef WLED_DISABLE_MQTT
    void publishHomeAssistantAutodiscovery();
#endif

    template<typename ... Args>
    std::string stringFormat( const std::string& format, Args ... args )
    {
        int size_s = std::snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
        if( size_s <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
        auto size = static_cast<size_t>( size_s );
        std::unique_ptr<char[]> buf( new char[ size ] );
        std::snprintf( buf.get(), size, format.c_str(), args ... );
        return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
    }

public:

    /*
     * API calls te enable data exchange between WLED modules
     */
    inline float getTemperatureC(const uint8_t index) {
        if (index >= sensorCount)
            return -127;
        return rawToCelsius(temperatures[index]);
    }
    inline float getTemperatureF(const uint8_t index) {
        if (index >= sensorCount)
            return -196.6;
        return rawToFahrenheit(temperatures[index]);
    }

    float getTemperature(const uint8_t index);
    const char *getTemperatureUnit();
    uint16_t getId() { return USERMOD_ID_CINEMAGIC_TEMPERATURE; }

    void setup();
    void loop();
    //void connected();
#ifndef WLED_DISABLE_MQTT
    void onMqttConnect(bool sessionPresent);
#endif
    //void onUpdateBegin(bool init);

    //bool handleButton(uint8_t b);
    //void handleOverlayDraw();

    void addToJsonInfo(JsonObject& root);
    //void addToJsonState(JsonObject &root);
    //void readFromJsonState(JsonObject &root);
    void addToConfig(JsonObject &root);
    bool readFromConfig(JsonObject &root);

    void appendConfigData();
};

void UsermodCinemagicTemperature::requestTemperatures() {
    if (waitingForConversion)
        return;
    DEBUG_PRINTLN(F("Requesting temperature."));
    oneWire->reset();
    oneWire->skip();                        // skip ROM
    oneWire->write(0x44,parasite);          // request new temperature reading
    if (parasite && parasitePin >=0 ) digitalWrite(parasitePin, HIGH); // has to happen within 10us (open MOSFET)
    lastTemperaturesRequest = millis();
    waitingForConversion = true;
}

void UsermodCinemagicTemperature::readAllTemperatures() {
    if (sensorCount == 0)
        return;

    if (millis() - lastTemperaturesRequest < 750 /* 93.75ms per the datasheet but can be up to 750ms */)
        return;

    for (uint8_t i = 0; i < sensorCount; i++){
        temperatures[i] = getRawTemperature(sensors[i]);
    }

    lastMeasurement = millis();
    waitingForConversion = false;
}

int32_t UsermodCinemagicTemperature::getRawTemperature(const uint8_t* deviceAddress) {
    ScratchPad scratchPad;
    if (isSensorConnected(deviceAddress, scratchPad))
        return calculateTemperature(deviceAddress, scratchPad);
    return TEMP_DEVICE_DISCONNECTED_RAW;
}

bool UsermodCinemagicTemperature::findSensors() {
    DEBUG_PRINTLN(F("Searching for sensor..."));
    DeviceAddress deviceAddress;
    // find out if we have DS18xxx sensor attached
    oneWire->reset_search();
    delay(10);
    while (oneWire->search(deviceAddress)) {
        if (oneWire->crc8(deviceAddress, 7) == deviceAddress[7]) {
            switch (deviceAddress[0]) {
                case DS18S20MODEL:  // DS18S20
                case DS1822MODEL:  // DS18B20
                case DS18B20MODEL:  // DS1822
                case DS1825MODEL:  // DS1825
                case DS28EA00MODEL:  // DS28EA00
                    sensorCount++;
            }
        }
    }

    oneWire->reset_search();
    delay(10);
    delete[] sensors;
    delete[] temperatures;
    sensors = new DeviceAddress[sensorCount];
    temperatures = new int32_t[sensorCount];
    uint8_t i = 0;
    while (oneWire->search(deviceAddress)) {
        DEBUG_PRINTLN(F("Found something..."));
        if (oneWire->crc8(deviceAddress, 7) == deviceAddress[7]) {
            switch (deviceAddress[0]) {
                case DS18S20MODEL:  // DS18S20
                case DS1822MODEL:  // DS18B20
                case DS18B20MODEL:  // DS1822
                case DS1825MODEL:  // DS1825
                case DS28EA00MODEL:  // DS28EA00
                    std::copy(std::begin(deviceAddress), std::end(deviceAddress), std::begin(sensors[i++]));
            }
        }
    }
    DEBUG_PRINTLN(F("Sensor NOT found."));
    return true;
}

// convert from raw to Celsius
float UsermodCinemagicTemperature::rawToCelsius(int32_t raw) {

    if (raw <= TEMP_DEVICE_DISCONNECTED_RAW)
        return -127;
    // C = RAW/128
    return (float) raw * 0.0078125f;

}

// convert from raw to Fahrenheit
float UsermodCinemagicTemperature::rawToFahrenheit(int32_t raw) {

    if (raw <= TEMP_DEVICE_DISCONNECTED_RAW)
        return -196.6;
    // C = RAW/128
    // F = (C*1.8)+32 = (RAW/128*1.8)+32 = (RAW*0.0140625)+32
    return ((float) raw * 0.0140625f) + 32.0f;

}

// reads scratchpad and returns fixed-point temperature, scaling factor 2^-7
int32_t UsermodCinemagicTemperature::calculateTemperature(const uint8_t* deviceAddress,
                                                uint8_t* scratchPad) {
    int32_t fpTemperature = 0;

    // looking thru the spec sheets of all supported devices, bit 15 is always the signing bit
    // Detected if signed
    int32_t neg = 0x0;
    if (scratchPad[1] & 0x80)
        neg = 0xFFF80000;

    // detect MAX31850
    // The temp range on a MAX31850 can far exceed other models, causing an overrun @ 256C
    // Based on the spec sheets for the MAX31850, bit 7 is always 1
    // Whereas the DS1825 bit 7 is always 0
    // DS1825   - https://datasheets.maximintegrated.com/en/ds/DS1825.pdf
    // MAX31850 - https://datasheets.maximintegrated.com/en/ds/MAX31850-MAX31851.pdf

    if (deviceAddress[0] == DS1825MODEL && scratchPad[4] & 0x80 ) {
        //Serial.print("  Detected MAX31850");
        if (scratchPad[0] & 1) { // Fault Detected
            if (scratchPad[2] & 1) {
                //Serial.println("open detected");
                return -32512;
            }
            else if (scratchPad[2] >> 1 & 1) {
                //Serial.println("short to ground detected");
                return -32384;
            }
            else if (scratchPad[2] >> 2 & 1) {
                //Serial.println("short to Vdd detected");
                return -32256;
            }
            else {
                // We don't know why there's a fault, exit with disconnect value
                return TEMP_DEVICE_DISCONNECTED_RAW;
            }
        }
        // We must mask out bit 1 (reserved) and 0 (fault) on TEMP_LSB
        fpTemperature = (((int32_t) scratchPad[1]) << 11)
                        | (((int32_t) scratchPad[0] & 0xFC) << 3)
                        | neg;
    } else {
        fpTemperature = (((int16_t) scratchPad[1]) << 11)
                        | (((int16_t) scratchPad[0]) << 3)
                        | neg;
    }

    if ((deviceAddress[0] == DS18S20MODEL) && (scratchPad[7] != 0)) {
        fpTemperature = ((fpTemperature & 0xfff0) << 3) - 32
                        + (((scratchPad[7] - scratchPad[6]) << 7)
                           / scratchPad[7]);
    }

    return fpTemperature;
}

// attempt to determine if the device at the given address is connected to the bus
bool UsermodCinemagicTemperature::isSensorConnected(const uint8_t* deviceAddress) {

    ScratchPad scratchPad;
    return isSensorConnected(deviceAddress, scratchPad);

}

// attempt to determine if the device at the given address is connected to the bus
// also allows for updating the read scratchpad
bool UsermodCinemagicTemperature::isSensorConnected(const uint8_t* deviceAddress,
                                    uint8_t* scratchPad) {
    bool b = readScratchPad(deviceAddress, scratchPad);
    return b && !isAllZeros(scratchPad) && (oneWire->crc8(scratchPad, 8) == scratchPad[8]);
}

// Returns true if all bytes of scratchPad are '\0'
bool UsermodCinemagicTemperature::isAllZeros(const uint8_t * const scratchPad, const size_t length) {
    for (size_t i = 0; i < length; i++) {
        if (scratchPad[i] != 0) {
            return false;
        }
    }
    return true;
}

bool UsermodCinemagicTemperature::readScratchPad(const uint8_t* deviceAddress,
                                       uint8_t* scratchPad) {
    // send the reset command and fail fast
    int b = oneWire->reset();
    if (b == 0)
        return false;

    oneWire->select(deviceAddress);
    oneWire->write(0xBE);

    // Read all registers in a simple loop
    // byte 0: temperature LSB
    // byte 1: temperature MSB
    // byte 2: high alarm temp
    // byte 3: low alarm temp
    // byte 4: DS18S20: store for crc
    //         DS18B20 & DS1822: configuration register
    // byte 5: internal use & crc
    // byte 6: DS18S20: COUNT_REMAIN
    //         DS18B20 & DS1822: store for crc
    // byte 7: DS18S20: COUNT_PER_C
    //         DS18B20 & DS1822: store for crc
    // byte 8: SCRATCHPAD_CRC
    for (uint8_t i = 0; i < 9; i++) {
        scratchPad[i] = oneWire->read();
    }

    b = oneWire->reset();
    return (b == 1);
}

#ifndef WLED_DISABLE_MQTT
void UsermodCinemagicTemperature::publishHomeAssistantAutodiscovery() {
    if (!WLED_MQTT_CONNECTED) return;

    char json_str[1024], buf[128];
    size_t payload_size;
    StaticJsonDocument<1024> json;

    sprintf_P(buf, PSTR("%s Temperature"), serverDescription);
    json[F("name")] = buf;
    strcpy(buf, mqttDeviceTopic);
    strcat_P(buf, PSTR("/temperature"));
    json[F("state_topic")] = buf;
    json[F("device_class")] = F("temperature");
    json[F("unique_id")] = escapedMac.c_str();
    json[F("unit_of_measurement")] = F("°C");
    payload_size = serializeJson(json, json_str);

    sprintf_P(buf, PSTR("homeassistant/sensor/%s/config"), escapedMac.c_str());
    mqtt->publish(buf, 0, true, json_str, payload_size);
    HApublished = true;
}
#endif

void UsermodCinemagicTemperature::setup() {
    int retries = 10;
    sensorCount = 0;
    delete[] sensors;

    if (enabled) {
        // config says we are enabled
        DEBUG_PRINTLN(F("Allocating temperature pin..."));
        // pin retrieved from cfg.json (readFromConfig()) prior to running setup()
        if (temperaturePin >= 0 && pinManager.allocatePin(temperaturePin, true, PinOwner::UM_Temperature)) {
            oneWire = new OneWire(temperaturePin);
            if (oneWire->reset()) {
                while (!findSensors() && retries--) {
                    delay(25); // try to find sensor
                }
            }
            if (parasite && pinManager.allocatePin(parasitePin, true, PinOwner::UM_Temperature)) {
                pinMode(parasitePin, OUTPUT);
                digitalWrite(parasitePin, LOW); // deactivate power (close MOSFET)
            } else {
                parasitePin = -1;
            }
        } else {
            if (temperaturePin >= 0) {
                DEBUG_PRINTLN(F("Temperature pin allocation failed."));
            }
            temperaturePin = -1;  // allocation failed
        }
    }
    lastMeasurement = millis() - readingInterval - 1;
    initDone = true;
}

void UsermodCinemagicTemperature::loop() {
    if (!enabled || sensorCount == 0 || strip.isUpdating()) return;

    // check to see if we are due for taking a measurement
    // lastMeasurement will not be updated until the conversion
    // is complete the reading is finished
    if (millis() - lastMeasurement < readingInterval) return;

    // we are due for a measurement, if we are not already waiting
    // for a conversion to complete, then make a new request for temps
    requestTemperatures();
    readAllTemperatures();

#ifndef WLED_DISABLE_MQTT
//        if (WLED_MQTT_CONNECTED) {
//            char subuf[64];
//            strcpy(subuf, mqttDeviceTopic);
//            if (temperature > -100.0f) {
//                // dont publish super low temperature as the graph will get messed up
//                // the DallasTemperature library returns -127C or -196.6F when problem
//                // reading the sensor
//                strcat_P(subuf, PSTR("/temperature"));
//                mqtt->publish(subuf, 0, false, String(getTemperatureC()).c_str());
//                strcat_P(subuf, PSTR("_f"));
//                mqtt->publish(subuf, 0, false, String(getTemperatureF()).c_str());
//            } else {
//                // publish something else to indicate status?
//            }
//        }
#endif
}

/**
 * connected() is called every time the WiFi is (re)connected
 * Use it to initialize network interfaces
 */
//void UsermodCinemagicTemperature::connected() {}

#ifndef WLED_DISABLE_MQTT
/**
 * subscribe to MQTT topic if needed
 */
void UsermodCinemagicTemperature::onMqttConnect(bool sessionPresent) {
    //(re)subscribe to required topics
    //char subuf[64];
    if (mqttDeviceTopic[0] != 0) {
        publishHomeAssistantAutodiscovery();
    }
}
#endif

/*
  * addToJsonInfo() can be used to add custom entries to the /json/info part of the JSON API.
  * Creating an "u" object allows you to add custom key/value pairs to the Info section of the WLED web UI.
  * Below it is shown how this could be used for e.g. a light sensor
  */
void UsermodCinemagicTemperature::addToJsonInfo(JsonObject& root) {
    // dont add temperature to info if we are disabled
    if (!enabled) return;

    JsonObject user = root["u"];
    if (user.isNull()) user = root.createNestedObject("u");

    JsonObject sensor = root[F("sensor")];
    if (sensor.isNull()) sensor = root.createNestedObject(F("sensor"));

    for (uint8_t i = 0; i < sensorCount; i++){
        std::string sensorName = stringFormat("Temperature %d", (i+1));
        JsonArray temp = user.createNestedArray(sensorName);
        const float temperature = (int)(getTemperature(i) * 100) / 100.f;
        if (temperature <= -100.0f) {
            temp.add(0);
            temp.add(F(" Sensor Error!"));
            return;
        }
        temp.add(temperature);
        temp.add(getTemperatureUnit());

        temp = sensor.createNestedArray(sensorName);
        temp.add(temperature);
        temp.add(getTemperatureUnit());
    }

}

/**
 * addToJsonState() can be used to add custom entries to the /json/state part of the JSON API (state object).
 * Values in the state object may be modified by connected clients
 */
//void UsermodCinemagicTemperature::addToJsonState(JsonObject &root)
//{
//}

/**
 * readFromJsonState() can be used to receive data clients send to the /json/state part of the JSON API (state object).
 * Values in the state object may be modified by connected clients
 * Read "<usermodname>_<usermodparam>" from json state and and change settings (i.e. GPIO pin) used.
 */
//void UsermodCinemagicTemperature::readFromJsonState(JsonObject &root) {
//  if (!initDone) return;  // prevent crash on boot applyPreset()
//}

/**
 * addToConfig() (called from set.cpp) stores persistent properties to cfg.json
 */
void UsermodCinemagicTemperature::addToConfig(JsonObject &root) {
    // we add JSON object: {"Temperature": {"pin": 0, "degC": true}}
    JsonObject top = root.createNestedObject(FPSTR(_name)); // usermodname
    top[FPSTR(_enabled)] = enabled;
    top["pin"]  = temperaturePin;     // usermodparam
    top["degC"] = degC;  // usermodparam
    top[FPSTR(_readInterval)] = readingInterval / 1000;
    top[FPSTR(_parasite)] = parasite;
    top[FPSTR(_parasitePin)] = parasitePin;
    DEBUG_PRINTLN(F("Temperature config saved."));
}

/**
 * readFromConfig() is called before setup() to populate properties from values stored in cfg.json
 *
 * The function should return true if configuration was successfully loaded or false if there was no configuration.
 */
bool UsermodCinemagicTemperature::readFromConfig(JsonObject &root) {
    // we look for JSON object: {"Temperature": {"pin": 0, "degC": true}}
    int8_t newTemperaturePin = temperaturePin;
    DEBUG_PRINT(FPSTR(_name));

    JsonObject top = root[FPSTR(_name)];
    if (top.isNull()) {
        DEBUG_PRINTLN(F(": No config found. (Using defaults.)"));
        return false;
    }

    enabled           = top[FPSTR(_enabled)] | enabled;
    newTemperaturePin = top["pin"] | newTemperaturePin;
    degC              = top["degC"] | degC;
    readingInterval   = top[FPSTR(_readInterval)];
    readingInterval   *= 1000;  // convert to ms
    parasite          = top[FPSTR(_parasite)] | parasite;
    parasitePin       = top[FPSTR(_parasitePin)] | parasitePin;

    if (readingInterval == 0)
        readingInterval = USERMOD_DALLASTEMPERATURE_MEASUREMENT_INTERVAL;

    if (!initDone) {
        // first run: reading from cfg.json
        temperaturePin = newTemperaturePin;
        DEBUG_PRINTLN(F(" config loaded."));
    } else {
        DEBUG_PRINTLN(F(" config (re)loaded."));
        // changing paramters from settings page
        if (newTemperaturePin != temperaturePin) {
            DEBUG_PRINTLN(F("Re-init temperature."));
            // deallocate pin and release memory
            delete oneWire;
            pinManager.deallocatePin(temperaturePin, PinOwner::UM_Temperature);
            temperaturePin = newTemperaturePin;
            pinManager.deallocatePin(parasitePin, PinOwner::UM_Temperature);
            // initialise
            setup();
        }
    }
    // use "return !top["newestParameter"].isNull();" when updating Usermod with new features
    return !top[FPSTR(_parasitePin)].isNull();
}

void UsermodCinemagicTemperature::appendConfigData() {
    oappend(SET_F("addInfo('")); oappend(String(FPSTR(_name)).c_str()); oappend(SET_F(":")); oappend(String(FPSTR(_parasite)).c_str());
    oappend(SET_F("',1,'<i>(if no Vcc connected)</i>');"));  // 0 is field type, 1 is actual field
    oappend(SET_F("addInfo('")); oappend(String(FPSTR(_name)).c_str()); oappend(SET_F(":")); oappend(String(FPSTR(_parasitePin)).c_str());
    oappend(SET_F("',1,'<i>(for external MOSFET)</i>');"));  // 0 is field type, 1 is actual field
}

float UsermodCinemagicTemperature::getTemperature(const uint8_t index) {
    return degC ? getTemperatureC(index) : getTemperatureF(index);
}

const char *UsermodCinemagicTemperature::getTemperatureUnit() {
    return degC ? "°C" : "°F";
}

// strings to reduce flash memory usage (used more than twice)
const char UsermodCinemagicTemperature::_name[]         PROGMEM = "CinemagicTemperature";
const char UsermodCinemagicTemperature::_enabled[]      PROGMEM = "enabled";
const char UsermodCinemagicTemperature::_readInterval[] PROGMEM = "read-interval-s";
const char UsermodCinemagicTemperature::_parasite[]     PROGMEM = "parasite-pwr";
const char UsermodCinemagicTemperature::_parasitePin[]  PROGMEM = "parasite-pwr-pin";
