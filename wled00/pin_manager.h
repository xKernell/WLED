#ifndef WLED_PIN_MANAGER_H
#define WLED_PIN_MANAGER_H
/*
 * Registers pins so there is no attempt for two interfaces to use the same pin
 */

#ifdef ESP8266
#define WLED_NUM_PINS (GPIO_PIN_COUNT+1) // somehow they forgot GPIO 16 (0-16==17)
#else
#define WLED_NUM_PINS (GPIO_PIN_COUNT)
#endif

typedef struct PinManagerPinType {
  int8_t pin;
  bool   isOutput;
} managed_pin_type;

/*
 * Allows PinManager to "lock" an allocation to a specific
 * owner, so someone else doesn't accidentally de-allocate
 * a pin it hasn't allocated.  Also enhances debugging.
 *
 * RAM Cost:
 *     17 bytes on ESP8266
 *     40 bytes on ESP32
 */
enum struct PinOwner : uint8_t {
  None          = 0,      // default == legacy == unspecified owner
  // High bit is set for all built-in pin owners
  Ethernet      = 0x81,
  BusDigital    = 0x82,
  BusOnOff      = 0x83,
  BusPwm        = 0x84,   // 'BusP'      == PWM output using BusPwm
  Button        = 0x85,   // 'Butn'      == button from configuration
  IR            = 0x86,   // 'IR'        == IR receiver pin from configuration
  Relay         = 0x87,   // 'Rly'       == Relay pin from configuration
  SPI_RAM       = 0x88,   // 'SpiR'      == SPI RAM
  DebugOut      = 0x89,   // 'Dbg'       == debug output always IO1
  DMX           = 0x8A,   // 'DMX'       == hard-coded to IO2
  HW_I2C        = 0x8B,   // 'I2C'       == hardware I2C pins (4&5 on ESP8266, 21&22 on ESP32)
  HW_SPI        = 0x8C,   // 'SPI'       == hardware (V)SPI pins (13,14&15 on ESP8266, 5,18&23 on ESP32)
  DMX_INPUT     = 0x8D,   // 'DMX_INPUT' == DMX input via serial
  // Use UserMod IDs from const.h here
  UM_Unspecified       = USERMOD_ID_UNSPECIFIED,        // 0x01
  UM_Example           = USERMOD_ID_EXAMPLE,            // 0x02 // Usermod "usermod_v2_example.h"
  UM_Temperature       = USERMOD_ID_TEMPERATURE,        // 0x03 // Usermod "usermod_temperature.h"
  // #define USERMOD_ID_FIXNETSERVICES                  // 0x04 // Usermod "usermod_Fix_unreachable_netservices.h" -- Does not allocate pins
  UM_PIR               = USERMOD_ID_PIRSWITCH,          // 0x05 // Usermod "usermod_PIR_sensor_switch.h"
  UM_IMU               = USERMOD_ID_IMU,                // 0x06 // Usermod "usermod_mpu6050_imu.h" -- Interrupt pin
  UM_FourLineDisplay   = USERMOD_ID_FOUR_LINE_DISP,     // 0x07 // Usermod "usermod_v2_four_line_display.h -- May use "standard" HW_I2C pins
  UM_RotaryEncoderUI   = USERMOD_ID_ROTARY_ENC_UI,      // 0x08 // Usermod "usermod_v2_rotary_encoder_ui.h"
  // #define USERMOD_ID_AUTO_SAVE                       // 0x09 // Usermod "usermod_v2_auto_save.h" -- Does not allocate pins
  // #define USERMOD_ID_DHT                             // 0x0A // Usermod "usermod_dht.h" -- Statically allocates pins, not compatible with pinManager?
  // #define USERMOD_ID_VL53L0X                         // 0x0C // Usermod "usermod_vl53l0x_gestures.h" -- Uses "standard" HW_I2C pins
  UM_MultiRelay        = USERMOD_ID_MULTI_RELAY,        // 0x0D // Usermod "usermod_multi_relay.h"
  UM_AnimatedStaircase = USERMOD_ID_ANIMATED_STAIRCASE, // 0x0E // Usermod "Animated_Staircase.h"
  UM_Battery           = USERMOD_ID_BATTERY,            //
  // #define USERMOD_ID_RTC                             // 0x0F // Usermod "usermod_rtc.h" -- Uses "standard" HW_I2C pins
  // #define USERMOD_ID_ELEKSTUBE_IPS                   // 0x10 // Usermod "usermod_elekstube_ips.h" -- Uses quite a few pins ... see Hardware.h and User_Setup.h
  // #define USERMOD_ID_SN_PHOTORESISTOR                // 0x11 // Usermod "usermod_sn_photoresistor.h" -- Uses hard-coded pin (PHOTORESISTOR_PIN == A0), but could be easily updated to use pinManager
  UM_BH1750            = USERMOD_ID_BH1750,             // 0x14 // Usermod "bh1750.h -- Uses "standard" HW_I2C pins
  UM_RGBRotaryEncoder  = USERMOD_RGB_ROTARY_ENCODER,    // 0x16 // Usermod "rgb-rotary-encoder.h"
  UM_QuinLEDAnPenta    = USERMOD_ID_QUINLED_AN_PENTA,   // 0x17 // Usermod "quinled-an-penta.h"
  UM_BME280            = USERMOD_ID_BME280,             // 0x1E // Usermod "usermod_bme280.h -- Uses "standard" HW_I2C pins
  UM_Audioreactive     = USERMOD_ID_AUDIOREACTIVE,      // 0x20 // Usermod "audio_reactive.h"
  UM_SdCard            = USERMOD_ID_SD_CARD,            // 0x25 // Usermod "usermod_sd_card.h"
  UM_PWM_OUTPUTS       = USERMOD_ID_PWM_OUTPUTS,        // 0x26 // Usermod "usermod_pwm_outputs.h"
  UM_LDR_DUSK_DAWN     = USERMOD_ID_LDR_DUSK_DAWN,      // 0x2B // Usermod "usermod_LDR_Dusk_Dawn_v2.h"
  UM_MAX17048          = USERMOD_ID_MAX17048,           // 0x2F // Usermod "usermod_max17048.h"
  UM_BME68X            = USERMOD_ID_BME68X,             // 0x31 // Usermod "usermod_bme68x.h -- Uses "standard" HW_I2C pins
  UM_PIXELS_DICE_TRAY  = USERMOD_ID_PIXELS_DICE_TRAY,   // 0x35 // Usermod "pixels_dice_tray.h" -- Needs compile time specified 6 pins for display including SPI.
  UM_CINEMAGIC         = USERMOD_ID_CINEMAGIC
};
static_assert(0u == static_cast<uint8_t>(PinOwner::None), "PinOwner::None must be zero, so default array initialization works as expected");

namespace PinManager {
  // De-allocates a single pin
  bool deallocatePin(byte gpio, PinOwner tag);
  // De-allocates multiple pins but only if all can be deallocated (PinOwner has to be specified)
  bool deallocateMultiplePins(const uint8_t *pinArray, byte arrayElementCount, PinOwner tag);
  bool deallocateMultiplePins(const managed_pin_type *pinArray, byte arrayElementCount, PinOwner tag);
  // Allocates a single pin, with an owner tag.
  // De-allocation requires the same owner tag (or override)
  bool allocatePin(byte gpio, bool output, PinOwner tag);
  // Allocates all the pins, or allocates none of the pins, with owner tag.
  // Provided to simplify error condition handling in clients
  // using more than one pin, such as I2C, SPI, rotary encoders,
  // ethernet, etc..
  bool allocateMultiplePins(const managed_pin_type * mptArray, byte arrayElementCount, PinOwner tag );

  [[deprecated("Replaced by three-parameter allocatePin(gpio, output, ownerTag), for improved debugging")]]
  inline bool allocatePin(byte gpio, bool output = true) { return allocatePin(gpio, output, PinOwner::None); }
  [[deprecated("Replaced by two-parameter deallocatePin(gpio, ownerTag), for improved debugging")]]
  inline void deallocatePin(byte gpio) { deallocatePin(gpio, PinOwner::None); }

  // will return true for reserved pins
  bool isPinAllocated(byte gpio, PinOwner tag = PinOwner::None);
  // will return false for reserved pins
  bool isPinOk(byte gpio, bool output = true);

  bool isReadOnlyPin(byte gpio);

  PinOwner getPinOwner(byte gpio);

  #ifdef ARDUINO_ARCH_ESP32
  byte allocateLedc(byte channels);
  void deallocateLedc(byte pos, byte channels);
  #endif
};

//extern PinManager pinManager;
#endif
