//
// Created by Mohsen Basiti on 2024-11-11.
//

#pragma once
#ifdef USERMOD_CINEMAGIC_AUDIO
#include "cinemagic_shared.h"

#ifndef USERMOD_CINEMAGIC_AUDIO_ADDR
#define USERMOD_CINEMAGIC_AUDIO_ADDR 0x10 // AD1=0, AD0=0
#endif

class CMAudio {
public:
    CMShared *shared;

    CMAudio(CMShared *sh) : shared(sh) {};

    void begin();
    void loop();

private:
    bool i2cWrite(uint8_t reg, uint8_t val);
};

void CMAudio::begin() {
    if (!i2cWrite(0x00, 0xFF)) return;

    delay(10);

    // 2) Power-down all blocks first
    i2cWrite(0x01, 0x3F);           // PDN_ADC | PDN_VREF | DIS_DIG
    delay(5);

    // 3) Set I²S slave, 24-bit, LRCK/BCK = 64 × Fs
    i2cWrite(0x02, 0x00);           // I2S mode, SDTO polarity normal
    i2cWrite(0x03, 0x00);           // LRCK div = 64 fs

    // 4) Enable stereo ADC, VREF on
    i2cWrite(0x01, 0x00);           // all power-ups
    delay(10);

    DEBUG_PRINTLN("CINEMAGIC AUDIO Inited Successfully!");
}

void CMAudio::loop() {

}

bool CMAudio::i2cWrite(uint8_t reg, uint8_t val){
    DEBUG_PRINTLN("CINEMAGIC AUDIO i2c start transmitting ...");
    Wire.beginTransmission(USERMOD_CINEMAGIC_AUDIO_ADDR);
    Wire.write(reg);
    Wire.write(val);
    bool res = Wire.endTransmission() == 0;
    if (!res){
        DEBUG_PRINTLN("CINEMAGIC AUDIO i2c transmission failed!");
    } else {
        DEBUG_PRINTLN("CINEMAGIC AUDIO i2c transmission completed!");
    }
    return res;
}

#endif