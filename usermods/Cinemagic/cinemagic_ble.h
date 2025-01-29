//
// Created by Mohsen Basiti on 2024-11-10.
//
#pragma once
#ifdef USERMOD_CINEMAGIC_BLE

#include "cinemagic_shared.h"
#include "wled.h"

// Use the official Arduino BLE library:
#include <ArduinoBLE.h>

class CMBLE {
public:
    explicit CMBLE(CMShared *sh) : shared(sh) {}

    void begin();
    void loop();

private:
    CMShared *shared;

    // Create a BLE Service with UUID "1234" (example)
    BLEService wledService{"1234"};

    // Create a read characteristic (UUID "ABCD") for reading WLED state
    // We'll allocate enough space (up to 512 bytes) to hold the JSON string
    BLECharacteristic readCharacteristic {
        "ABCD",
        BLERead,
        512
    };

    // Create a write characteristic (UUID "EF01") for incoming JSON-based commands
    BLECharacteristic writeCharacteristic {
        "EF01",
        BLEWrite | BLEWriteWithoutResponse,
        512
    };

    // Helper method to send the current WLED JSON state to the read characteristic
    void updateReadCharacteristic();

    // Helper method to handle new data written to the write characteristic
    void handleWrite();
};


void CMBLE::begin() {
    Serial.println("BLEControlUsermod: Initializing Arduino BLE...");

    if (!BLE.begin()) {
        Serial.println("BLEControlUsermod: starting BLE failed!");
        return;
    }

    // Set a local/device name that the phone sees when scanning
    BLE.setLocalName("WLED-BLE");
    BLE.setDeviceName("WLED-BLE");

    // Add characteristics to our service
    wledService.addCharacteristic(readCharacteristic);
    wledService.addCharacteristic(writeCharacteristic);

    // Add the service to the BLE stack
    BLE.addService(wledService);

    // (Optional) write the initial WLED state into the read characteristic
    updateReadCharacteristic();

    // Start advertising our service
    BLE.advertise();

    Serial.println("BLEControlUsermod: BLE service started (UUID: 1234).");
}


void CMBLE::loop() {
    // Let ArduinoBLE process events (connections, reads/writes, etc.)
    BLE.poll();

    // Check if a central device is connected
    BLEDevice central = BLE.central();
    if (!central) {
        // No central connected, nothing else to do now.
        return;
    }

    // While connected to a BLE central
    if (writeCharacteristic.written()) {
        handleWrite();
        updateReadCharacteristic();
    }
}


void CMBLE::updateReadCharacteristic() {
    // Serialize the current WLED state to JSON
    DynamicJsonDocument doc(4096);
    JsonObject root = doc.to<JsonObject>();
    serializeState(root);  // Provided by WLED, populates `doc` with the state

    // Convert JSON to string
    String jsonOut;
    serializeJson(doc, jsonOut);

    // Update the BLE read characteristic with the new JSON
    readCharacteristic.writeValue(jsonOut.c_str());

    Serial.println("BLEControlUsermod: Updated readCharacteristic with WLED JSON state:");
    Serial.println(jsonOut);
}


void CMBLE::handleWrite() {
    // Retrieve the raw string that was written to the characteristic
    const uint8_t* data = writeCharacteristic.value();   // raw pointer to data
    size_t length       = writeCharacteristic.valueLength(); // how many bytes

    if (length == 0) {
      Serial.println("BLEControlUsermod: Received an empty write, ignoring.");
      return;
    }

    String value((const char*) data, length);


    // Debug print
    Serial.print("BLEControlUsermod: onWrite -> Received: ");
    Serial.println(value);

    // Parse the incoming JSON
    DynamicJsonDocument doc(4096);
    DeserializationError err = deserializeJson(doc, value);
    if (err) {
        Serial.println("BLEControlUsermod: JSON parse error!");
        return;
    }

    JsonObject root = doc.as<JsonObject>();

    // Update WLED from the incoming JSON (immediate effect)
    deserializeState(root, true, CALL_MODE_DIRECT_CHANGE);

    // Confirm
    Serial.println("BLEControlUsermod: WLED state updated via BLE JSON command.");
}

#endif // USERMOD_CINEMAGIC_BLE
