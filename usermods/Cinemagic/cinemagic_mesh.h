#pragma once

#ifdef USERMOD_CINEMAGIC_MESH

#include <esp_now.h>
#include <esp_wifi.h>
#include "wled.h"
#include "cinemagic_shared.h"

// If WLED_FS not defined in your environment, define it for LittleFS or SPIFFS:
#ifndef WLED_FS
#include <FS.h>
#include <LittleFS.h>
#define WLED_FS LittleFS
#endif

#ifndef MESH_ID_FILE
#define MESH_ID_FILE      "/meshid.dat" // file storing the device's unique ID
#endif

#ifndef MESH_ID_LENGTH
#define MESH_ID_LENGTH    16  // e.g. 16 hex digits
#endif

#ifndef MY_ACCOUNT_ID
#define MY_ACCOUNT_ID     "noaccount@example.com"
#endif

#ifndef MY_DEVICE_GROUP
#define MY_DEVICE_GROUP   0  // range 0..7
#endif

// Simple struct for broadcast. Just brightness, but you can add effect, color, etc.
struct CinemagicMeshPacket {
    char  accountID[32];                   // must match MY_ACCOUNT_ID
    uint8_t group;                         // must match MY_DEVICE_GROUP
    char  deviceID[MESH_ID_LENGTH + 1];    // sender's ID
    CMControl control;
};

class CMMesh {
public:
    CMShared* shared;

    // Our unique ID
    char deviceID[MESH_ID_LENGTH+1] = {0};

    // We'll only call esp_now_send() if this is true
    bool meshReady = false;

    // We'll do an initial check for Wi-Fi on first loop
    bool firstCheck = true;

    explicit CMMesh(CMShared* sh) : shared(sh) {}

    // Called once in usermod setup()
    void begin() {
        DEBUG_PRINTLN(F("[Mesh] begin() called."));
        // 1) FS init
        if(!WLED_FS.begin(true)) {
            DEBUG_PRINTLN(F("[Mesh] FS mount failed! Possibly no FS or mount error."));
        } else {
            DEBUG_PRINTLN(F("[Mesh] FS mounted OK."));
        }

        // 2) Load or generate device ID
        if(!loadDeviceID()) {
            generateDeviceID();
            saveDeviceID();
        }
        DEBUG_PRINT(F("[Mesh] My deviceID: "));
        DEBUG_PRINTLN(deviceID);
        // We'll do actual esp-now setup in loop() after we see what Wi-Fi mode is
    }

    // Called regularly in usermod loop
    void loop() {
        // We'll do an initial check or re-check if we haven't inited yet
        if(!meshReady || firstCheck) {
            checkInitEspNow();
        }
    }

    // Called by WLED when brightness/effect changes
    void onStateChanged(uint8_t mode) {
        // If not ready, skip
        if(!meshReady) {
            DEBUG_PRINTLN(F("[Mesh] Not ready, skipping broadcast."));
            return;
        }
        // Build packet
        CinemagicMeshPacket pkt;
        memset(&pkt, 0, sizeof(pkt));
        strncpy(pkt.accountID, MY_ACCOUNT_ID, sizeof(pkt.accountID)-1);
        pkt.group = MY_DEVICE_GROUP;
        strncpy(pkt.deviceID, deviceID, MESH_ID_LENGTH);
        pkt.control = shared->control; // from WLED
        broadcastPacket(pkt);
    }

    void onWiFiConnected() {
        // Called once Wi-Fi is definitely connected.
        // The channel might have changed from 6 to router's channel.

        // If we are still meshReady, remove the old broadcast peer:
        if (meshReady && havePeer) {
            // remove old peer
            uint8_t bcMac[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
            esp_now_del_peer(bcMac);
            havePeer = false;
            DEBUG_PRINTLN(F("[Mesh] Removed old broadcast peer after Wi-Fi connect."));
        }

        // re-add the peer with the new channel
        if (meshReady) {
            addBroadcastPeer(); // calls esp_wifi_get_channel() for the up-to-date channel
            DEBUG_PRINTLN(F("[Mesh] Re-added broadcast peer with new channel!"));
        }
    }

private:
    static CMMesh* instance;
    bool havePeer = false; // track if we already added broadcast peer

    // ---------- File-based device ID ----------

    bool loadDeviceID() {
        File f = WLED_FS.open(MESH_ID_FILE, "r");
        if(!f) {
            DEBUG_PRINTLN(F("[Mesh] No mesh ID file found, will generate new ID."));
            return false;
        }
        size_t n = f.readBytes(deviceID, MESH_ID_LENGTH);
        f.close();
        deviceID[n] = '\0';
        if(n != MESH_ID_LENGTH) {
            DEBUG_PRINTLN(F("[Mesh] Mesh ID file incomplete, regenerating."));
            return false;
        }
        DEBUG_PRINTLN(F("[Mesh] Loaded mesh ID from FS."));
        return true;
    }

    void saveDeviceID() {
        File f = WLED_FS.open(MESH_ID_FILE, "w");
        if(!f) {
            DEBUG_PRINTLN(F("[Mesh] Could not open mesh ID file for writing!"));
            return;
        }
        size_t written = f.write((const uint8_t*)deviceID, MESH_ID_LENGTH);
        f.close();
        if(written == MESH_ID_LENGTH) {
            DEBUG_PRINTLN(F("[Mesh] Mesh ID saved to FS."));
        } else {
            DEBUG_PRINTLN(F("[Mesh] Failed to write complete mesh ID."));
        }
    }

    void generateDeviceID() {
        for(int i=0; i<MESH_ID_LENGTH; i++){
            uint8_t r = random(0,16);
            deviceID[i] = "0123456789ABCDEF"[r];
        }
        deviceID[MESH_ID_LENGTH] = '\0';
        DEBUG_PRINTLN(F("[Mesh] Generated a new random deviceID."));
    }

    // ---------- Checking Wi-Fi mode & init ESP-NOW ----------

    void checkInitEspNow() {
        firstCheck = false;
        // We'll see if WLED is connected to a router or in fallback AP
        bool connected = (WiFi.status() == WL_CONNECTED);

        wifi_mode_t mode;
        esp_wifi_get_mode(&mode);
        DEBUG_PRINT(F("[Mesh] checkInitEspNow() -> WiFi mode: "));
        DEBUG_PRINT(mode);
        DEBUG_PRINT(F(", connected? "));
        DEBUG_PRINTLN(connected);

        if(connected) {
            // We have STA. Typically we also have AP if WLED is in AP_STA.
            // Force AP_STA if not set.
            if(!(mode & WIFI_MODE_STA)) {
                DEBUG_PRINTLN(F("[Mesh] Forcing WIFI_AP_STA because we are connected but not STA?"));
                WiFi.mode(WIFI_AP_STA);
            }
        } else {
            // fallback AP => let's forcibly do AP+STA with a fixed channel
            if(!(mode & WIFI_MODE_STA)) {
                DEBUG_PRINTLN(F("[Mesh] In fallback AP, forcing WIFI_AP_STA + channel 6."));
                WiFi.mode(WIFI_AP_STA);
            }
            // pick channel 6
            esp_err_t cRes = esp_wifi_set_channel(6, WIFI_SECOND_CHAN_NONE);
            DEBUG_PRINT(F("[Mesh] set channel 6 => "));
            DEBUG_PRINTLN(cRes);
        }

        // Now init ESP‑NOW if not done
        if(!meshReady) {
            esp_err_t inRes = esp_now_init();
            if(inRes==ESP_OK) {
                DEBUG_PRINTLN(F("[Mesh] esp_now_init() success."));
                esp_now_register_recv_cb(onRecvStatic);
                addBroadcastPeer(); // set broadcast peer
                instance = this;
                meshReady = true;
            } else {
                DEBUG_PRINT(F("[Mesh] esp_now_init() failed with code "));
                DEBUG_PRINTLN(inRes);
            }
        }
    }

    void addBroadcastPeer() {
        if(havePeer) return;
        esp_now_peer_info_t peerInfo = {};
        memset(&peerInfo, 0, sizeof(peerInfo));
        for(int i=0; i<6; i++){
            peerInfo.peer_addr[i] = 0xFF; // broadcast
        }
        // set channel to current
        wifi_second_chan_t sc;
        uint8_t ch;
        esp_wifi_get_channel(&ch, &sc);
        peerInfo.channel = ch;
        peerInfo.encrypt = false;
        esp_err_t r = esp_now_add_peer(&peerInfo);
        if(r==ESP_OK) {
            DEBUG_PRINT(F("[Mesh] Broadcast peer added on channel "));
            DEBUG_PRINTLN(ch);
            havePeer = true;
        } else {
            DEBUG_PRINT(F("[Mesh] add_peer error: "));
            DEBUG_PRINTLN(r);
        }
    }

    // ---------- Sending ----------

    void broadcastPacket(const CinemagicMeshPacket &pkt) {
        if(!meshReady || !havePeer) {
            DEBUG_PRINTLN(F("[Mesh] Not ready or no peer, skipping broadcastPacket."));
            return;
        }
        uint8_t bcMac[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
        esp_err_t err = esp_now_send(bcMac, (const uint8_t*)&pkt, sizeof(pkt));
        if(err != ESP_OK) {
            DEBUG_PRINT(F("[Mesh] Send error: "));
            DEBUG_PRINTLN(err);
        } else {
            DEBUG_PRINTLN(F("[Mesh] Packet broadcast success!"));
        }
    }

    // ---------- Receiving ----------

    static void onRecvStatic(const uint8_t * mac, const uint8_t *data, int len) {
        if(instance) {
            instance->handleIncoming(mac, data, len);
        }
    }

    void handleIncoming(const uint8_t * mac, const uint8_t *data, int len) {
        if(len < (int)sizeof(CinemagicMeshPacket)) {
            DEBUG_PRINTLN(F("[Mesh] Incoming packet too small, ignoring."));
            return;
        }

        CinemagicMeshPacket pkt;
        memcpy(&pkt, data, sizeof(pkt));

        if(strcmp(pkt.accountID, MY_ACCOUNT_ID) != 0) {
            DEBUG_PRINTLN(F("[Mesh] Different accountID, ignoring packet."));
            return;
        }
        if(pkt.group != MY_DEVICE_GROUP) {
            DEBUG_PRINTLN(F("[Mesh] Different group, ignoring packet."));
            return;
        }
        if(strncmp(pkt.deviceID, deviceID, MESH_ID_LENGTH)==0) {
            // same device => ignore to avoid loop
            DEBUG_PRINTLN(F("[Mesh] Packet from self, ignoring."));
            return;
        }

        // If we get here, we accept the brightness
        DEBUG_PRINT(F("[Mesh] Updating"));
        shared->control = pkt.control;
        cmUpdateStrip(shared, CM_CALL_MODE_ALL);
    }
};

// static pointer
CMMesh* CMMesh::instance = nullptr;

#endif // USERMOD_CINEMAGIC_MESH
