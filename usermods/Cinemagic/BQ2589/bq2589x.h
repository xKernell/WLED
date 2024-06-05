#pragma once
#include <Wire.h>

#define BQ25895_ADDR (0x6A);

#define I2C_OK 0 //0:success
#define I2C_ERR 1

#define SCL_PIN 5
#define SDA_PIN 4

#define BQ2589X_OK 0
#define BQ2589X_ERR 1 //  ERR>0
/* Register 00h */
#define BQ2589X_REG_00 0x00
#define BQ2589X_ENHIZ_MASK 0x80
#define BQ2589X_ENHIZ_SHIFT 7
#define BQ2589X_HIZ_ENABLE 1
#define BQ2589X_HIZ_DISABLE 0
#define BQ2589X_ENILIM_MASK 0x40
#define BQ2589X_ENILIM_SHIFT 6
#define BQ2589X_ENILIM_ENABLE 1
#define BQ2589X_ENILIM_DISABLE 0

#define BQ2589X_IINLIM_MASK 0x3F
#define BQ2589X_IINLIM_SHIFT 0
#define BQ2589X_IINLIM_BASE 100
#define BQ2589X_IINLIM_LSB 50

/* Register 01h */
#define BQ2589X_REG_01 0x01
#define BQ2589X_BHOT_MASK 0xC0
#define BQ2589X_BHOT_SHIFT 6
#define BQ2589X_BCOLD_MASK 0x20
#define BQ2589X_BCOLD_SHIFT 5
#define BQ2589X_VINDPMOS_MASK 0x1F
#define BQ2589X_VINDPMOS_SHIFT 0

#define BQ2589X_VINDPMOS_BASE 0
#define BQ2589X_VINDPMOS_LSB 100

/* Register 0x02 */
#define BQ2589X_REG_02 0x02
#define BQ2589X_CONV_START_MASK 0x80
#define BQ2589X_CONV_START_SHIFT 7
#define BQ2589X_CONV_START 1
#define BQ2589X_CONV_RATE_MASK 0x40
#define BQ2589X_CONV_RATE_SHIFT 6
#define BQ2589X_ADC_CONTINUE_ENABLE 1
#define BQ2589X_ADC_CONTINUE_DISABLE 0

#define BQ2589X_BOOST_FREQ_MASK 0x20
#define BQ2589X_BOOST_FREQ_SHIFT 5
#define BQ2589X_BOOST_FREQ_1500K 0
#define BQ2589X_BOOST_FREQ_500K 0

#define BQ2589X_ICOEN_MASK 0x10
#define BQ2589X_ICOEN_SHIFT 4
#define BQ2589X_ICO_ENABLE 1
#define BQ2589X_ICO_DISABLE 0
#define BQ2589X_HVDCPEN_MASK 0x08
#define BQ2589X_HVDCPEN_SHIFT 3
#define BQ2589X_HVDCP_ENABLE 1
#define BQ2589X_HVDCP_DISABLE 0
#define BQ2589X_MAXCEN_MASK 0x04
#define BQ2589X_MAXCEN_SHIFT 2
#define BQ2589X_MAXC_ENABLE 1
#define BQ2589X_MAXC_DISABLE 0

#define BQ2589X_FORCE_DPDM_MASK 0x02
#define BQ2589X_FORCE_DPDM_SHIFT 1
#define BQ2589X_FORCE_DPDM 1
#define BQ2589X_AUTO_DPDM_EN_MASK 0x01
#define BQ2589X_AUTO_DPDM_EN_SHIFT 0
#define BQ2589X_AUTO_DPDM_ENABLE 1
#define BQ2589X_AUTO_DPDM_DISABLE 0

/* Register 0x03 */
#define BQ2589X_REG_03 0x03
#define BQ2589X_BAT_LOADEN_MASK 0x80
#define BQ2589X_BAT_LOAEN_SHIFT 7
#define BQ2589X_WDT_RESET_MASK 0x40
#define BQ2589X_WDT_RESET_SHIFT 6
#define BQ2589X_WDT_RESET 1

#define BQ2589X_OTG_CONFIG_MASK 0x20
#define BQ2589X_OTG_CONFIG_SHIFT 5
#define BQ2589X_OTG_ENABLE 1
#define BQ2589X_OTG_DISABLE 0

#define BQ2589X_CHG_CONFIG_MASK 0x10
#define BQ2589X_CHG_CONFIG_SHIFT 4
#define BQ2589X_CHG_ENABLE 1
#define BQ2589X_CHG_DISABLE 0

#define BQ2589X_SYS_MINV_MASK 0x0E
#define BQ2589X_SYS_MINV_SHIFT 1

#define BQ2589X_SYS_MINV_BASE 3000
#define BQ2589X_SYS_MINV_LSB 100

/* Register 0x04*/
#define BQ2589X_REG_04 0x04
#define BQ2589X_EN_PUMPX_MASK 0x80
#define BQ2589X_EN_PUMPX_SHIFT 7
#define BQ2589X_PUMPX_ENABLE 1
#define BQ2589X_PUMPX_DISABLE 0
#define BQ2589X_ICHG_MASK 0x7F
#define BQ2589X_ICHG_SHIFT 0
#define BQ2589X_ICHG_BASE 0
#define BQ2589X_ICHG_LSB 64

/* Register 0x05*/
#define BQ2589X_REG_05 0x05
#define BQ2589X_IPRECHG_MASK 0xF0
#define BQ2589X_IPRECHG_SHIFT 4
#define BQ2589X_ITERM_MASK 0x0F
#define BQ2589X_ITERM_SHIFT 0
#define BQ2589X_IPRECHG_BASE 64
#define BQ2589X_IPRECHG_LSB 64
#define BQ2589X_ITERM_BASE 64
#define BQ2589X_ITERM_LSB 64

/* Register 0x06*/
#define BQ2589X_REG_06 0x06
#define BQ2589X_VREG_MASK 0xFC //0xFC
#define BQ2589X_VREG_SHIFT 2
#define BQ2589X_BATLOWV_MASK 0x02
#define BQ2589X_BATLOWV_SHIFT 1
#define BQ2589X_BATLOWV_2800MV 0
#define BQ2589X_BATLOWV_3000MV 1
#define BQ2589X_VRECHG_MASK 0x01
#define BQ2589X_VRECHG_SHIFT 0
#define BQ2589X_VRECHG_100MV 0
#define BQ2589X_VRECHG_200MV 1
#define BQ2589X_VREG_BASE 3840
#define BQ2589X_VREG_LSB 16

/* Register 0x07*/
#define BQ2589X_REG_07 0x07
#define BQ2589X_EN_TERM_MASK 0x80
#define BQ2589X_EN_TERM_SHIFT 7
#define BQ2589X_TERM_ENABLE 1
#define BQ2589X_TERM_DISABLE 0

#define BQ2589X_WDT_MASK 0x30
#define BQ2589X_WDT_SHIFT 4
#define BQ2589X_WDT_DISABLE 0
#define BQ2589X_WDT_40S 1
#define BQ2589X_WDT_80S 2
#define BQ2589X_WDT_160S 3
#define BQ2589X_WDT_BASE 0
#define BQ2589X_WDT_LSB 40

#define BQ2589X_EN_TIMER_MASK 0x08
#define BQ2589X_EN_TIMER_SHIFT 3

#define BQ2589X_CHG_TIMER_ENABLE 1
#define BQ2589X_CHG_TIMER_DISABLE 0

#define BQ2589X_CHG_TIMER_MASK 0x06
#define BQ2589X_CHG_TIMER_SHIFT 1
#define BQ2589X_CHG_TIMER_5HOURS 0
#define BQ2589X_CHG_TIMER_8HOURS 1
#define BQ2589X_CHG_TIMER_12HOURS 2
#define BQ2589X_CHG_TIMER_20HOURS 3

#define BQ2589X_JEITA_ISET_MASK 0x01
#define BQ2589X_JEITA_ISET_SHIFT 0
#define BQ2589X_JEITA_ISET_50PCT 0
#define BQ2589X_JEITA_ISET_20PCT 1

/* Register 0x08*/
#define BQ2589X_REG_08 0x08
#define BQ2589X_BAT_COMP_MASK 0xE0
#define BQ2589X_BAT_COMP_SHIFT 5
#define BQ2589X_VCLAMP_MASK 0x1C
#define BQ2589X_VCLAMP_SHIFT 2
#define BQ2589X_TREG_MASK 0x03
#define BQ2589X_TREG_SHIFT 0
#define BQ2589X_TREG_60C 0
#define BQ2589X_TREG_80C 1
#define BQ2589X_TREG_100C 2
#define BQ2589X_TREG_120C 3

#define BQ2589X_BAT_COMP_BASE 0
#define BQ2589X_BAT_COMP_LSB 20
#define BQ2589X_VCLAMP_BASE 0
#define BQ2589X_VCLAMP_LSB 32

/* Register 0x09*/
#define BQ2589X_REG_09 0x09
#define BQ2589X_FORCE_ICO_MASK 0x80
#define BQ2589X_FORCE_ICO_SHIFT 7
#define BQ2589X_FORCE_ICO 1
#define BQ2589X_TMR2X_EN_MASK 0x40
#define BQ2589X_TMR2X_EN_SHIFT 6
#define BQ2589X_BATFET_DIS_MASK 0x20
#define BQ2589X_BATFET_DIS_SHIFT 5
#define BQ2589X_BATFET_OFF 1

#define BQ2589X_JEITA_VSET_MASK 0x10
#define BQ2589X_JEITA_VSET_SHIFT 4
#define BQ2589X_JEITA_VSET_N150MV 0
#define BQ2589X_JEITA_VSET_VREG 1
#define BQ2589X_BATFET_RST_EN_MASK 0x04
#define BQ2589X_BATFET_RST_EN_SHIFT 2
#define BQ2589X_PUMPX_UP_MASK 0x02
#define BQ2589X_PUMPX_UP_SHIFT 1
#define BQ2589X_PUMPX_UP 1
#define BQ2589X_PUMPX_DOWN_MASK 0x01
#define BQ2589X_PUMPX_DOWN_SHIFT 0
#define BQ2589X_PUMPX_DOWN 1

/* Register 0x0A*/
#define BQ2589X_REG_0A 0x0A
#define BQ2589X_BOOSTV_MASK 0xF0
#define BQ2589X_BOOSTV_SHIFT 4
#define BQ2589X_BOOSTV_BASE 4550
#define BQ2589X_BOOSTV_LSB 64

#define BQ2589X_BOOST_LIM_MASK 0x07
#define BQ2589X_BOOST_LIM_SHIFT 0
#define BQ2589X_BOOST_LIM_500MA 0x00
#define BQ2589X_BOOST_LIM_700MA 0x01
#define BQ2589X_BOOST_LIM_1100MA 0x02
#define BQ2589X_BOOST_LIM_1300MA 0x03
#define BQ2589X_BOOST_LIM_1600MA 0x04
#define BQ2589X_BOOST_LIM_1800MA 0x05
#define BQ2589X_BOOST_LIM_2100MA 0x06
#define BQ2589X_BOOST_LIM_2400MA 0x07

/* Register 0x0B*/
#define BQ2589X_REG_0B 0x0B
#define BQ2589X_VBUS_STAT_MASK 0xE0
#define BQ2589X_VBUS_STAT_SHIFT 5
#define BQ2589X_CHRG_STAT_MASK 0x18
#define BQ2589X_CHRG_STAT_SHIFT 3
#define BQ2589X_CHRG_STAT_IDLE 0
#define BQ2589X_CHRG_STAT_PRECHG 1
#define BQ2589X_CHRG_STAT_FASTCHG 2
#define BQ2589X_CHRG_STAT_CHGDONE 3

#define BQ2589X_PG_STAT_MASK 0x04
#define BQ2589X_PG_STAT_SHIFT 2
#define BQ2589X_SDP_STAT_MASK 0x02
#define BQ2589X_SDP_STAT_SHIFT 1
#define BQ2589X_VSYS_STAT_MASK 0x01
#define BQ2589X_VSYS_STAT_SHIFT 0

/* Register 0x0C*/
#define BQ2589X_REG_0C 0x0c
#define BQ2589X_FAULT_WDT_MASK 0x80
#define BQ2589X_FAULT_WDT_SHIFT 7
#define BQ2589X_FAULT_BOOST_MASK 0x40
#define BQ2589X_FAULT_BOOST_SHIFT 6
#define BQ2589X_FAULT_CHRG_MASK 0x30
#define BQ2589X_FAULT_CHRG_SHIFT 4
#define BQ2589X_FAULT_CHRG_NORMAL 0
#define BQ2589X_FAULT_CHRG_INPUT 1
#define BQ2589X_FAULT_CHRG_THERMAL 2
#define BQ2589X_FAULT_CHRG_TIMER 3

#define BQ2589X_FAULT_BAT_MASK 0x08
#define BQ2589X_FAULT_BAT_SHIFT 3
#define BQ2589X_FAULT_NTC_MASK 0x07
#define BQ2589X_FAULT_NTC_SHIFT 0
#define BQ2589X_FAULT_NTC_TSCOLD 1
#define BQ2589X_FAULT_NTC_TSHOT 2

#define BQ2589X_FAULT_NTC_WARM 2
#define BQ2589X_FAULT_NTC_COOL 3
#define BQ2589X_FAULT_NTC_COLD 5
#define BQ2589X_FAULT_NTC_HOT 6

/* Register 0x0D*/
#define BQ2589X_REG_0D 0x0D
#define BQ2589X_FORCE_VINDPM_MASK 0x80
#define BQ2589X_FORCE_VINDPM_SHIFT 7
#define BQ2589X_FORCE_VINDPM_ENABLE 1
#define BQ2589X_FORCE_VINDPM_DISABLE 0
#define BQ2589X_VINDPM_MASK 0x7F
#define BQ2589X_VINDPM_SHIFT 0

#define BQ2589X_VINDPM_BASE 2600
#define BQ2589X_VINDPM_LSB 100

/* Register 0x0E*/
#define BQ2589X_REG_0E 0x0E
#define BQ2589X_THERM_STAT_MASK 0x80
#define BQ2589X_THERM_STAT_SHIFT 7
#define BQ2589X_BATV_MASK 0x7F
#define BQ2589X_BATV_SHIFT 0
#define BQ2589X_BATV_BASE 2304
#define BQ2589X_BATV_LSB 20

/* Register 0x0F*/
#define BQ2589X_REG_0F 0x0F
#define BQ2589X_SYSV_MASK 0x7F
#define BQ2589X_SYSV_SHIFT 0
#define BQ2589X_SYSV_BASE 2304
#define BQ2589X_SYSV_LSB 20

/* Register 0x10*/
#define BQ2589X_REG_10 0x10
#define BQ2589X_TSPCT_MASK 0x7F
#define BQ2589X_TSPCT_SHIFT 0
#define BQ2589X_TSPCT_BASE 21
#define BQ2589X_TSPCT_LSB 0.465 //should be 0.465,kernel does not support float

/* Register 0x11*/
#define BQ2589X_REG_11 0x11
#define BQ2589X_VBUS_GD_MASK 0x80
#define BQ2589X_VBUS_GD_SHIFT 7
#define BQ2589X_VBUSV_MASK 0x7F
#define BQ2589X_VBUSV_SHIFT 0
#define BQ2589X_VBUSV_BASE 2600
#define BQ2589X_VBUSV_LSB 100

/* Register 0x12*/
#define BQ2589X_REG_12 0x12
#define BQ2589X_ICHGR_MASK 0x7F
#define BQ2589X_ICHGR_SHIFT 0
#define BQ2589X_ICHGR_BASE 0
#define BQ2589X_ICHGR_LSB 50

/* Register 0x13*/
#define BQ2589X_REG_13 0x13
#define BQ2589X_VDPM_STAT_MASK 0x80
#define BQ2589X_VDPM_STAT_SHIFT 7
#define BQ2589X_IDPM_STAT_MASK 0x40
#define BQ2589X_IDPM_STAT_SHIFT 6
#define BQ2589X_IDPM_LIM_MASK 0x3F
#define BQ2589X_IDPM_LIM_SHIFT 0
#define BQ2589X_IDPM_LIM_BASE 100
#define BQ2589X_IDPM_LIM_LSB 50

/* Register 0x14*/
#define BQ2589X_REG_14 0x14
#define BQ2589X_RESET_MASK 0x80
#define BQ2589X_RESET_SHIFT 7
#define BQ2589X_RESET 1
#define BQ2589X_ICO_OPTIMIZED_MASK 0x40
#define BQ2589X_ICO_OPTIMIZED_SHIFT 6
#define BQ2589X_PN_MASK 0x38
#define BQ2589X_PN_SHIFT 3
#define BQ2589X_TS_PROFILE_MASK 0x04
#define BQ2589X_TS_PROFILE_SHIFT 2
#define BQ2589X_DEV_REV_MASK 0x03
#define BQ2589X_DEV_REV_SHIFT 0

typedef enum bq2589x_vbus_type
{
    BQ2589X_VBUS_NONE,
    BQ2589X_VBUS_USB_SDP,
    BQ2589X_VBUS_USB_CDP, /*CDP for bq25890, Adapter for bq25892*/
    BQ2589X_VBUS_USB_DCP,
    BQ2589X_VBUS_MAXC,
    BQ2589X_VBUS_UNKNOWN,
    BQ2589X_VBUS_NONSTAND,
    BQ2589X_VBUS_OTG,
    BQ2589X_VBUS_TYPE_NUM,
} bq2589x_vbus_type;

typedef enum bq2589x_part_no
{
    BQ25890 = 0x03,
    BQ25892 = 0x00,
    BQ25895 = 0x07,
} bq2589x_part_no;



class bq2589x
{
private:
    TwoWire *_wire;
    uint8_t _i2caddr;

    /* data */
public:
    bq2589x(/* args */);
    ~bq2589x();
    int begin();
    int begin(TwoWire *theWire);
    int begin(uint8_t addr);
    int begin(uint8_t addr, TwoWire *theWire);
    int read_byte(uint8_t *data, uint8_t reg);
    int write_byte(uint8_t reg, uint8_t data);
    int update_bits(uint8_t reg, uint8_t mask, uint8_t data);
    bq2589x_vbus_type get_vbus_type();
    int enable_otg();
    int disable_otg();
    int set_otg_volt(uint16_t volt);
    int set_otg_current(int curr);
    int enable_charger();
    int disable_charger();
    int adc_start(bool oneshot);
    int adc_stop();
    int adc_read_battery_volt();
    int adc_read_sys_volt();
    int adc_read_vbus_volt();
    int adc_read_temperature();
    int adc_read_charge_current();
    int set_charge_current(int curr);
    int set_term_current(int curr);
    int set_prechg_current(int curr);
    int set_chargevoltage(int volt);
    int set_input_volt_limit(int volt);
    int set_input_current_limit(int curr);
    int set_vindpm_offset(int offset);
    int get_charging_status();
    void bq2589x_set_otg(int enable);
    int set_watchdog_timer(uint8_t timeout);
    int disable_watchdog_timer();
    int reset_watchdog_timer();
    int force_dpdm();
    int reset_chip();
    int enter_ship_mode();
    int enter_hiz_mode();
    int exit_hiz_mode();
    int get_hiz_mode(uint8_t *state);
    int pumpx_enable(int enable);
    int pumpx_increase_volt();
    int pumpx_increase_volt_done();
    int pumpx_decrease_volt();
    int pumpx_decrease_volt_done();
    int force_ico();
    int check_force_ico_done();
    int enable_term(bool enable);
    int enable_auto_dpdm(bool enable);
    int use_absolute_vindpm(bool enable);
    int enable_ico(bool enable);
    int read_idpm_limit();
    bool is_charge_done();
    int init_device();
    int detect_device(bq2589x_part_no *part_no, int *revision);
    int enable_max_charge(bool enable);
};


bq2589x::bq2589x(/* args */)
{
}

bq2589x::~bq2589x()
{
}

/*!
 *   @brief  Initialise sensor with given parameters / settings
 *   @param addr the I2C address the device can be found on
 *   @returns true on success, false otherwise
 */
int bq2589x::begin(uint8_t addr)
{
    _i2caddr = addr;
    _wire = &Wire;
    return reset_chip();
}

/*!
 *   @brief  Initialise sensor with given parameters / settings
 *   @param addr the I2C address the device can be found on
 *   @param theWire the I2C object to use
 *   @returns true on success, false otherwise
 */
int bq2589x::begin(uint8_t addr, TwoWire *theWire)
{
    _i2caddr = addr;
    _wire = theWire;
    return reset_chip();
}

/*!
 *   @brief  Initialise sensor with given parameters / settings
 *   @returns true on success, false otherwise
 */
int bq2589x::begin(void)
{

    _i2caddr = BQ25895_ADDR;
    _wire = &Wire;
    return reset_chip();
}

int bq2589x::begin(TwoWire *theWire)
{
    _wire = theWire;
    _i2caddr = BQ25895_ADDR;
    return reset_chip();
}

int bq2589x::read_byte(uint8_t *data, uint8_t reg)
{
    int rtn;
    _wire->beginTransmission((uint8_t)_i2caddr);
    _wire->write(reg);
    rtn = _wire->endTransmission();

    if (rtn != I2C_OK) {
        Serial.print("I2C write error: ");
        Serial.println(rtn);
        return BQ2589X_ERR;
    }

    _wire->requestFrom((uint8_t)_i2caddr, (uint8_t)1);
    if (_wire->available()) {
        *data = _wire->read();
    } else {
        Serial.println("I2C read error: No data available");
        return BQ2589X_ERR;
    }
//
//    Serial.print("Read from reg 0x");
//    Serial.print(reg, HEX);
//    Serial.print(": 0x");
//    Serial.println(*data, HEX);

    return BQ2589X_OK;
}
int bq2589x::write_byte(uint8_t reg, uint8_t data)
{
    int rtn;
    _wire->beginTransmission((uint8_t)_i2caddr);
    _wire->write((uint8_t)reg);
    _wire->write((uint8_t)data);
    rtn = _wire->endTransmission();
//
//    Serial.print("Write to reg 0x");
//    Serial.print(reg, HEX);
//    Serial.print(": 0x");
//    Serial.println(data, HEX);

    if (rtn == I2C_OK)
    {
        return BQ2589X_OK; // TI lib uses 1 as failed
    }
    else
    {
        Serial.print("I2C write error: ");
        Serial.println(rtn);
        return BQ2589X_ERR; // TI lib uses 1 as failed
    }
}

int bq2589x::update_bits(uint8_t reg, uint8_t mask, uint8_t data)
{
    int ret;
    uint8_t tmp;

    ret = read_byte(&tmp, reg);

    if (ret)
        return ret;

    tmp &= ~mask;
    tmp |= data & mask;

    return write_byte(reg, tmp);
}

bq2589x_vbus_type bq2589x::get_vbus_type()
{
    uint8_t val = 0;
    int ret;

    ret = read_byte(&val, BQ2589X_REG_0B);
    if (ret)
        return (BQ2589X_VBUS_UNKNOWN);
    val &= BQ2589X_VBUS_STAT_MASK;
    val >>= BQ2589X_VBUS_STAT_SHIFT;

    return (bq2589x_vbus_type)val;
}

int bq2589x::enable_otg()
{
    uint8_t val = BQ2589X_OTG_ENABLE << BQ2589X_OTG_CONFIG_SHIFT;

    return update_bits(BQ2589X_REG_03,
                       BQ2589X_OTG_CONFIG_MASK, val);
}

int bq2589x::disable_otg()
{
    uint8_t val = BQ2589X_OTG_DISABLE << BQ2589X_OTG_CONFIG_SHIFT;

    return update_bits(BQ2589X_REG_03,
                       BQ2589X_OTG_CONFIG_MASK, val);
}

int bq2589x::set_otg_volt(uint16_t volt)
{
    uint8_t val = 0;

    if (volt < BQ2589X_BOOSTV_BASE)
        volt = BQ2589X_BOOSTV_BASE;
    if (volt > BQ2589X_BOOSTV_BASE + (BQ2589X_BOOSTV_MASK >> BQ2589X_BOOSTV_SHIFT) * BQ2589X_BOOSTV_LSB)
        volt = BQ2589X_BOOSTV_BASE + (BQ2589X_BOOSTV_MASK >> BQ2589X_BOOSTV_SHIFT) * BQ2589X_BOOSTV_LSB;

    val = ((volt - BQ2589X_BOOSTV_BASE) / BQ2589X_BOOSTV_LSB) << BQ2589X_BOOSTV_SHIFT;

    return update_bits(BQ2589X_REG_0A, BQ2589X_BOOSTV_MASK, val);
}

int bq2589x::set_otg_current(int curr)
{
    uint8_t temp;

    if (curr == 500)
        temp = BQ2589X_BOOST_LIM_500MA;
    else if (curr == 700)
        temp = BQ2589X_BOOST_LIM_700MA;
    else if (curr == 1100)
        temp = BQ2589X_BOOST_LIM_1100MA;
    else if (curr == 1600)
        temp = BQ2589X_BOOST_LIM_1600MA;
    else if (curr == 1800)
        temp = BQ2589X_BOOST_LIM_1800MA;
    else if (curr == 2100)
        temp = BQ2589X_BOOST_LIM_2100MA;
    else if (curr == 2400)
        temp = BQ2589X_BOOST_LIM_2400MA;
    else
        temp = BQ2589X_BOOST_LIM_1300MA;

    return update_bits(BQ2589X_REG_0A, BQ2589X_BOOST_LIM_MASK, temp << BQ2589X_BOOST_LIM_SHIFT);
}

int bq2589x::enable_charger()
{
    uint8_t val = BQ2589X_CHG_ENABLE << BQ2589X_CHG_CONFIG_SHIFT;
    return update_bits(BQ2589X_REG_03, BQ2589X_CHG_CONFIG_MASK, val);
}

int bq2589x::disable_charger()
{
    uint8_t val = BQ2589X_CHG_DISABLE << BQ2589X_CHG_CONFIG_SHIFT;
    return update_bits(BQ2589X_REG_03, BQ2589X_CHG_CONFIG_MASK, val);
}

/* interfaces that can be called by other module */
int bq2589x::adc_start(bool oneshot)
{
    uint8_t val;
    int ret;

    ret = read_byte(&val, BQ2589X_REG_02);
    if (ret)
    {
        return ret;
    }

    if (((val & BQ2589X_CONV_RATE_MASK) >> BQ2589X_CONV_RATE_SHIFT) == BQ2589X_ADC_CONTINUE_ENABLE)
        return BQ2589X_OK; /*is doing continuous scan*/
    if (oneshot)
        ret = update_bits(BQ2589X_REG_02, BQ2589X_CONV_START_MASK, BQ2589X_CONV_START << BQ2589X_CONV_START_SHIFT);
    else
        ret = update_bits(BQ2589X_REG_02, BQ2589X_CONV_RATE_MASK, BQ2589X_ADC_CONTINUE_ENABLE << BQ2589X_CONV_RATE_SHIFT);
    return ret;
}

int bq2589x::adc_stop()
{
    return update_bits(BQ2589X_REG_02, BQ2589X_CONV_RATE_MASK, BQ2589X_ADC_CONTINUE_DISABLE << BQ2589X_CONV_RATE_SHIFT);
}

int bq2589x::adc_read_battery_volt()
{
    uint8_t val;
    int volt;
    int ret;
    ret = read_byte(&val, BQ2589X_REG_0E);  // Read the value from register 0E
    if (ret)
    {
        Serial.println("bq2589x::adc_read_battery_volt() - Error in reading BQ Voltage");
        return ret;
    }
    else
    {
        volt = BQ2589X_BATV_BASE + ((val & BQ2589X_BATV_MASK) >> BQ2589X_BATV_SHIFT) * BQ2589X_BATV_LSB;
        return volt;
    }
}


int bq2589x::adc_read_sys_volt()
{
    uint8_t val;
    int volt;
    int ret;
    ret = read_byte(&val, BQ2589X_REG_0F);
    if (ret)
    {
        return ret;
    }
    else
    {
        volt = BQ2589X_SYSV_BASE + ((val & BQ2589X_SYSV_MASK) >> BQ2589X_SYSV_SHIFT) * BQ2589X_SYSV_LSB;
        return volt;
    }
}

int bq2589x::adc_read_vbus_volt()
{
    uint8_t val;
    int volt;
    int ret;
    ret = read_byte(&val, BQ2589X_REG_11);
    if (ret)
    {
        return ret;
    }
    else
    {
        volt = BQ2589X_VBUSV_BASE + ((val & BQ2589X_VBUSV_MASK) >> BQ2589X_VBUSV_SHIFT) * BQ2589X_VBUSV_LSB;

        return volt;
    }
}

int bq2589x::adc_read_temperature()
{
    uint8_t val;
    int temp;
    int ret;
    ret = read_byte(&val, BQ2589X_REG_10);
    if (ret)
    {
        return ret;
    }
    else
    {
        temp = BQ2589X_TSPCT_BASE + ((val & BQ2589X_TSPCT_MASK) >> BQ2589X_TSPCT_SHIFT) * BQ2589X_TSPCT_LSB;
        return temp;
    }
}

int bq2589x::adc_read_charge_current()
{
    uint8_t val;
    int volt;
    int ret;
    ret = read_byte(&val, BQ2589X_REG_12);
    if (ret)
    {
        return ret;
    }
    else
    {
        volt = (int)(BQ2589X_ICHGR_BASE + ((val & BQ2589X_ICHGR_MASK) >> BQ2589X_ICHGR_SHIFT) * BQ2589X_ICHGR_LSB);
        return volt;
    }
}

int bq2589x::set_charge_current(int curr)
{
    uint8_t ichg;
    ichg = (curr - BQ2589X_ICHG_BASE) / BQ2589X_ICHG_LSB;
    return update_bits(BQ2589X_REG_04, BQ2589X_ICHG_MASK, ichg << BQ2589X_ICHG_SHIFT);
}

int bq2589x::set_term_current(int curr)
{
    uint8_t iterm;

    iterm = (curr - BQ2589X_ITERM_BASE) / BQ2589X_ITERM_LSB;

    return update_bits(BQ2589X_REG_05, BQ2589X_ITERM_MASK, iterm << BQ2589X_ITERM_SHIFT);
}

int bq2589x::set_prechg_current(int curr)
{
    uint8_t iprechg;

    iprechg = (curr - BQ2589X_IPRECHG_BASE) / BQ2589X_IPRECHG_LSB;

    return update_bits(BQ2589X_REG_05, BQ2589X_IPRECHG_MASK, iprechg << BQ2589X_IPRECHG_SHIFT);
}

int bq2589x::set_chargevoltage(int volt)
{
    uint8_t val;

    val = (volt - BQ2589X_VREG_BASE) / BQ2589X_VREG_LSB;
    return update_bits(BQ2589X_REG_06, BQ2589X_VREG_MASK, val << BQ2589X_VREG_SHIFT);
}

int bq2589x::set_input_volt_limit(int volt)
{
    uint8_t val;
    val = (volt - BQ2589X_VINDPM_BASE) / BQ2589X_VINDPM_LSB;
    return update_bits(BQ2589X_REG_0D, BQ2589X_VINDPM_MASK, val << BQ2589X_VINDPM_SHIFT);
}

int bq2589x::set_input_current_limit(int curr)
{
    uint8_t val;

    val = (curr - BQ2589X_IINLIM_BASE) / BQ2589X_IINLIM_LSB;
    return update_bits(BQ2589X_REG_00, BQ2589X_IINLIM_MASK, val << BQ2589X_IINLIM_SHIFT);
}

int bq2589x::set_vindpm_offset(int offset)
{
    uint8_t val;

    val = (offset - BQ2589X_VINDPMOS_BASE) / BQ2589X_VINDPMOS_LSB;
    return update_bits(BQ2589X_REG_01, BQ2589X_VINDPMOS_MASK, val << BQ2589X_VINDPMOS_SHIFT);
}

int bq2589x::get_charging_status()
{
    uint8_t val = 0;
    int ret;

    ret = read_byte(&val, BQ2589X_REG_0B);
    if (ret)
    {
        return 0x04; //Error
    }
    val &= BQ2589X_CHRG_STAT_MASK;
    val >>= BQ2589X_CHRG_STAT_SHIFT;
    return val;
}

void bq2589x::bq2589x_set_otg(int enable)
{
    int ret;

    if (enable)
    {
        ret = enable_otg();
        if (ret)
        {
            return;
        }
    }
    else
    {
        ret = disable_otg();
    }
}

int bq2589x::set_watchdog_timer(uint8_t timeout)
{
    return update_bits(BQ2589X_REG_07, BQ2589X_WDT_MASK, (uint8_t)((timeout - BQ2589X_WDT_BASE) / BQ2589X_WDT_LSB) << BQ2589X_WDT_SHIFT);
}

int bq2589x::disable_watchdog_timer()
{
    uint8_t val = BQ2589X_WDT_DISABLE << BQ2589X_WDT_SHIFT;

    return update_bits(BQ2589X_REG_07, BQ2589X_WDT_MASK, val);
}

int bq2589x::reset_watchdog_timer()
{
    uint8_t val = BQ2589X_WDT_RESET << BQ2589X_WDT_RESET_SHIFT;

    return update_bits(BQ2589X_REG_03, BQ2589X_WDT_RESET_MASK, val);
}

int bq2589x::force_dpdm()
{
    int ret;
    uint8_t val = BQ2589X_FORCE_DPDM << BQ2589X_FORCE_DPDM_SHIFT;

    ret = update_bits(BQ2589X_REG_02, BQ2589X_FORCE_DPDM_MASK, val);
    if (ret)
        return ret;

    //msleep(20);/*TODO: how much time needed to finish dpdm detect?*/
    return BQ2589X_OK;
}

int bq2589x::reset_chip()
{
    int ret;
    uint8_t val = BQ2589X_RESET << BQ2589X_RESET_SHIFT;

    ret = update_bits(BQ2589X_REG_14, BQ2589X_RESET_MASK, val);
    return ret;
}

int bq2589x::enter_ship_mode()
{
    int ret;
    uint8_t val = BQ2589X_BATFET_OFF << BQ2589X_BATFET_DIS_SHIFT;

    ret = update_bits(BQ2589X_REG_09, BQ2589X_BATFET_DIS_MASK, val);
    return ret;
}

int bq2589x::enter_hiz_mode()
{
    uint8_t val = BQ2589X_HIZ_ENABLE << BQ2589X_ENHIZ_SHIFT;

    return update_bits(BQ2589X_REG_00, BQ2589X_ENHIZ_MASK, val);
}

int bq2589x::exit_hiz_mode()
{

    uint8_t val = BQ2589X_HIZ_DISABLE << BQ2589X_ENHIZ_SHIFT;

    return update_bits(BQ2589X_REG_00, BQ2589X_ENHIZ_MASK, val);
}

int bq2589x::get_hiz_mode(uint8_t *state)
{
    uint8_t val;
    int ret;

    ret = read_byte(&val, BQ2589X_REG_00);
    if (ret)
        return ret;
    *state = (val & BQ2589X_ENHIZ_MASK) >> BQ2589X_ENHIZ_SHIFT;

    return BQ2589X_OK;
}

int bq2589x::pumpx_enable(int enable)
{
    uint8_t val;
    int ret;

    if (enable)
        val = BQ2589X_PUMPX_ENABLE << BQ2589X_EN_PUMPX_SHIFT;
    else
        val = BQ2589X_PUMPX_DISABLE << BQ2589X_EN_PUMPX_SHIFT;

    ret = update_bits(BQ2589X_REG_04, BQ2589X_EN_PUMPX_MASK, val);

    return ret;
}

int bq2589x::pumpx_increase_volt()
{
    uint8_t val;
    int ret;

    val = BQ2589X_PUMPX_UP << BQ2589X_PUMPX_UP_SHIFT;

    ret = update_bits(BQ2589X_REG_09, BQ2589X_PUMPX_UP_MASK, val);

    return ret;
}

int bq2589x::pumpx_increase_volt_done()
{
    uint8_t val;
    int ret;

    ret = read_byte(&val, BQ2589X_REG_09);
    if (ret)
        return ret;

    if (val & BQ2589X_PUMPX_UP_MASK)
        return BQ2589X_ERR; /* not finished*/
    else
        return BQ2589X_OK; /* pumpx up finished*/
}

int bq2589x::pumpx_decrease_volt()
{
    uint8_t val;
    int ret;

    val = BQ2589X_PUMPX_DOWN << BQ2589X_PUMPX_DOWN_SHIFT;

    ret = update_bits(BQ2589X_REG_09, BQ2589X_PUMPX_DOWN_MASK, val);

    return ret;
}

int bq2589x::pumpx_decrease_volt_done()
{
    uint8_t val;
    int ret;

    ret = read_byte(&val, BQ2589X_REG_09);
    if (ret)
        return ret;

    if (val & BQ2589X_PUMPX_DOWN_MASK)
        return BQ2589X_ERR; /* not finished*/
    else
        return BQ2589X_OK; /* pumpx down finished*/
}

int bq2589x::force_ico()
{
    uint8_t val;
    int ret;

    val = BQ2589X_FORCE_ICO << BQ2589X_FORCE_ICO_SHIFT;

    ret = update_bits(BQ2589X_REG_09, BQ2589X_FORCE_ICO_MASK, val);

    return ret;
}

int bq2589x::check_force_ico_done()
{
    uint8_t val;
    int ret;

    ret = read_byte(&val, BQ2589X_REG_14);
    if (ret)
        return ret;

    if (val & BQ2589X_ICO_OPTIMIZED_MASK)
        return BQ2589X_ERR; /*finished*/
    else
        return BQ2589X_OK; /* in progress*/
}

int bq2589x::enable_term(bool enable)
{
    uint8_t val;
    int ret;

    if (enable)
        val = BQ2589X_TERM_ENABLE << BQ2589X_EN_TERM_SHIFT;
    else
        val = BQ2589X_TERM_DISABLE << BQ2589X_EN_TERM_SHIFT;

    ret = update_bits(BQ2589X_REG_07, BQ2589X_EN_TERM_MASK, val);

    return ret;
}

int bq2589x::enable_auto_dpdm(bool enable)
{
    uint8_t val;
    int ret;

    if (enable)
        val = BQ2589X_AUTO_DPDM_ENABLE << BQ2589X_AUTO_DPDM_EN_SHIFT;
    else
        val = BQ2589X_AUTO_DPDM_DISABLE << BQ2589X_AUTO_DPDM_EN_SHIFT;

    ret = update_bits(BQ2589X_REG_02, BQ2589X_AUTO_DPDM_EN_MASK, val);

    return ret;
}

int bq2589x::use_absolute_vindpm(bool enable)
{
    uint8_t val;
    int ret;

    if (enable)
        val = BQ2589X_FORCE_VINDPM_ENABLE << BQ2589X_FORCE_VINDPM_SHIFT;
    else
        val = BQ2589X_FORCE_VINDPM_DISABLE << BQ2589X_FORCE_VINDPM_SHIFT;

    ret = update_bits(BQ2589X_REG_0D, BQ2589X_FORCE_VINDPM_MASK, val);

    return ret;
}

int bq2589x::enable_ico(bool enable)
{
    uint8_t val;
    int ret;

    if (enable)
        val = BQ2589X_ICO_ENABLE << BQ2589X_ICOEN_SHIFT;
    else
        val = BQ2589X_ICO_DISABLE << BQ2589X_ICOEN_SHIFT;

    ret = update_bits(BQ2589X_REG_02, BQ2589X_ICOEN_MASK, val);

    return ret;
}

int bq2589x::read_idpm_limit()
{
    uint8_t val;
    int curr;
    int ret;

    ret = read_byte(&val, BQ2589X_REG_13);
    if (ret)
    {
        return ret;
    }
    else
    {
        curr = BQ2589X_IDPM_LIM_BASE + ((val & BQ2589X_IDPM_LIM_MASK) >> BQ2589X_IDPM_LIM_SHIFT) * BQ2589X_IDPM_LIM_LSB;
        return curr;
    }
}

bool bq2589x::is_charge_done()
{
    uint8_t val;

    read_byte(&val, BQ2589X_REG_0B);
    val &= BQ2589X_CHRG_STAT_MASK;
    val >>= BQ2589X_CHRG_STAT_SHIFT;

    return (bool)(val == BQ2589X_CHRG_STAT_CHGDONE);
}

int bq2589x::init_device()
{
    int ret;

    /*common initialization*/

    disable_watchdog_timer();

    ret = set_charge_current(2560); //2.5A

    return ret;
}

int bq2589x::detect_device(bq2589x_part_no *part_no, int *revision)
{
    uint8_t data;
    if (read_byte(&data, BQ2589X_REG_14) == 0)
    {
        *part_no = (bq2589x_part_no)((data & BQ2589X_PN_MASK) >> BQ2589X_PN_SHIFT);
        *revision = (data & BQ2589X_DEV_REV_MASK) >> BQ2589X_DEV_REV_SHIFT;
        return BQ2589X_OK;
    }
    return BQ2589X_ERR;
}

int bq2589x::enable_max_charge(bool enable)
{
    uint8_t val;
    uint8_t val1;
    int ret;

    if (enable)
    {
        val = BQ2589X_HVDCP_ENABLE << BQ2589X_HVDCPEN_SHIFT;
        val1 = BQ2589X_MAXC_ENABLE << BQ2589X_MAXCEN_SHIFT;
    }
    else
    {
        val = BQ2589X_HVDCP_DISABLE << BQ2589X_HVDCPEN_SHIFT;
        val1 = BQ2589X_MAXC_DISABLE << BQ2589X_MAXCEN_SHIFT;
    }

    ret = update_bits(BQ2589X_REG_02, BQ2589X_HVDCPEN_MASK, val);
    ret = update_bits(BQ2589X_REG_02, BQ2589X_MAXCEN_MASK, val1);

    return ret;
}
