/*******************************************************************************
 *
 *  name: STUSB4500.h
 *  date: Dec 31, 2019
 *  auth: ardnew <andrew@ardnew.com>
 *  desc: Main library interface source code
 *
 ******************************************************************************/

#ifndef __STUSB4500_H__
#define __STUSB4500_H__

// ----------------------------------------------------------------- includes --

#include <Wire.h>

/*******************************************************************************
 *
 *  name: STUSB4500_register.h
 *  date: Dec 31, 2019
 *  auth: ardnew <andrew@ardnew.com>
 *  desc: I2C register definitions for STUSB4500 (2019 STMicroelectronics)
 *
 ******************************************************************************/

#ifndef __LIBSTUSB4500_REGISTERS_H
#define __LIBSTUSB4500_REGISTERS_H

#ifdef __cplusplus
extern "C" {
#endif

/*************************************************************************************************************
  * @brief    STUSB_GEN1S ALERT_STATUS register Structure definition
  * @Address:   0Bh
  * @Access:  RC
  * @Note:  This register indicates an Alert has occurred.
  *             (When a bit value change occurs on one of the mentioned transition register, it automatically
  *             sets the corresponding alert bit in ALERT_STATUS register. )
  ************************************************************************************************************/
#define ALERT_STATUS_1 0x0B // Interrupt register
typedef union
{
    uint8_t d8;
    struct
    {
        uint8_t PHY_STATUS_AL          : 1;
        uint8_t PRT_STATUS_AL          : 1;
        uint8_t _Reserved_2            : 1;
        uint8_t PD_TYPEC_STATUS_AL     : 1;
        uint8_t HW_FAULT_STATUS_AL     : 1;
        uint8_t MONITORING_STATUS_AL   : 1;
        uint8_t CC_DETECTION_STATUS_AL : 1;
        uint8_t HARD_RESET_AL          : 1;
    } b;
} STUSB_GEN1S_ALERT_STATUS_RegTypeDef;

#define ALERT_STATUS_MASK 0x0C // interrupt MASK same mask as status should be used
typedef union
{
    uint8_t d8;
    struct
    {
        uint8_t PHY_STATUS_AL_MASK          : 1;
        uint8_t PRT_STATUS_AL_MASK          : 1;
        uint8_t _Reserved_2                 : 1;
        uint8_t PD_TYPEC_STATUS_AL_MASK     : 1;
        uint8_t HW_FAULT_STATUS_AL_MASK     : 1;
        uint8_t MONITORING_STATUS_AL_MASK   : 1;
        uint8_t CC_DETECTION_STATUS_AL_MASK : 1;
        uint8_t HARD_RESET_AL_MASK          : 1;
    } b;
} STUSB_GEN1S_ALERT_STATUS_MASK_RegTypeDef;

/*************************************************************************************************************
  * @brief:   STUSB_GEN1S CC_DETECTION_STATUS_TRANS register Structure definition
  * @Address:   0Dh
  * @Access:  RC
  * @Note:  This register indicates a bit value change has occurred in CC_DETECTION_STATUS register.
  ************************************************************************************************************/
#define PORT_STATUS_TRANS 0x0D
typedef union
{
    uint8_t d8;
    struct
    {
        uint8_t ATTACH_STATE_TRANS : 1;
        uint8_t _Reserved_1_7      : 7;
    } b;
} STUSB_GEN1S_CC_DETECTION_STATUS_TRANS_RegTypeDef;

#define STUSBMASK_ATTACH_STATUS_TRANS 0x01 //"0b: Cleared, 1b: Transition occurred on ATTACH_STATUS bit"

/*************************************************************************************************************
  * @brief:   STUSB_GEN1S CC_DETECTION_STATUS register Structure definition
  * @Address:   0Eh
  * @Access:  RO
  * @Note:  This register provides current status of the connection detection and corresponding operation modes.
  ************************************************************************************************************/
#define REG_PORT_STATUS 0x0E
typedef union
{
    uint8_t d8;
    struct
    {
        uint8_t CC_ATTACH_STATE       : 1;
        uint8_t CC_VCONN_SUPPLY_STATE : 1;
        uint8_t CC_DATA_ROLE          : 1;
        uint8_t CC_POWER_ROLE         : 1;
        uint8_t START_UP_POWER_MODE   : 1;
        uint8_t CC_ATTACH_MODE        : 3;
    } b;
} STUSB_GEN1S_CC_DETECTION_STATUS_RegTypeDef;

#define STUSBMASK_ATTACHED_STATUS 0x01
#define VALUE_NOT_ATTACHED        0x00
#define VALUE_ATTACHED            0x01

/*************************************************************************************************************
  * @brief:   STUSB_GEN1S MONITORING_STATUS_TRANS register Structure definition
  * @Address:   0Fh
  * @Access:  RC
  * @Note:  This register allows to:
  *              - Alert about any change that occurs in MONITORING_STATUS register.
  *              - Manage specific USB PD Acknowledge commands
  *              - to manage Type-C state machine Acknowledge to USB PD Requests commands
  ************************************************************************************************************/
#define TYPEC_MONITORING_STATUS_0 0x0F
typedef union
{
    uint8_t d8;
    struct
    {
        uint8_t VCONN_VALID_TRANS    : 1;
        uint8_t VBUS_VALID_SNK_TRANS : 1;
        uint8_t VBUS_VSAFE0V_TRANS   : 1;
        uint8_t VBUS_READY_TRANS     : 1;
        uint8_t VBUS_LOW_STATUS      : 1;
        uint8_t VBUS_HIGH_STATUS     : 1;
        uint8_t Reserved6_7          : 2;
    } b;
} STUSB_GEN1S_MONITORING_STATUS_TRANS_RegTypeDef;

/*************************************************************************************************************
  * @brief:   STUSB_GEN1S MONITORING_STATUS register Structure definition
  * @Address:   10h
  * @Access:  RO
  * @Note:  This register provides information on current status of the VBUS and VCONN voltages
  *   monitoring done respectively on VBUS_SENSE input pin and VCONN input pin.
  ************************************************************************************************************/
#define TYPEC_MONITORING_STATUS_1 0x10
typedef union
{
    uint8_t d8;
    struct
    {
        uint8_t VCONN_VALID    : 1;
        uint8_t VBUS_VALID_SNK : 1;
        uint8_t VBUS_VSAFE0V   : 1;
        uint8_t VBUS_READY     : 1;
        uint8_t _Reserved_4_7  : 4;
    } b;
} STUSB_GEN1S_MONITORING_STATUS_RegTypeDef;

#define CC_STATUS 0x11
typedef union
{
    uint8_t d8;
    struct
    {
        uint8_t CC1_STATE              : 2;
        uint8_t CC2_STATE              : 2;
        uint8_t CONNECT_RESULT         : 1;
        uint8_t LOOKING_FOR_CONNECTION : 1;
        uint8_t _Reserved_4_7          : 2;
    } b;
} STUSB_GEN1S_CC_STATUS_RegTypeDef;

/************************************************************************************************************
  * @brief:   STUSB_GEN1S HW_FAULT_STATUS_TRANS register Structure definition
  * @Address:   12h
  * @Access:  RC
  * @Note:  This register indicates a bit value change has occurred in HW_FAULT_STATUS register.
  *   It alerts also when the over temperature condition is met.
  ************************************************************************************************************/
#define CC_HW_FAULT_STATUS_0 0x12
typedef union
{
    uint8_t d8;
    struct
    {
        uint8_t VCONN_SW_OVP_FAULT_TRANS    : 1;
        uint8_t VCONN_SW_OCP_FAULT_TRANS    : 1;
        uint8_t VCONN_SW_RVP_FAULT_TRANS    : 1;
        uint8_t VBUS_VSRC_DISCH_FAULT_TRANS : 1;
        uint8_t VPU_VALID_TRANS             : 1;
        uint8_t VPU_OVP_FAULT_TRANS         : 1;
        uint8_t _Reserved_6                 : 1;
        uint8_t THERMAL_FAULT               : 1;
    } b;
} STUSB_GEN1S_HW_FAULT_STATUS_TRANS_RegTypeDef;

/************************************************************************************************************
  * @brief:   STUSB_GEN1S HW_FAULT_STATUS register Structure definition
  * @Address:   13h
  * @Access:  RO
  * @Note:  This register provides information on hardware fault conditions related to the
  *   internal pull-up voltage in Source power role and to the VCONN power switches
  ************************************************************************************************************/
#define CC_HW_FAULT_STATUS_1 0x13
typedef union
{
    uint8_t d8;
    struct
    {
        uint8_t VCONN_SW_OVP_FAULT : 1;
        uint8_t VCONN_SW_OCP_FAULT : 1;
        uint8_t VCONN_SW_RVP_FAULT : 1;
        uint8_t VSRC_DISCH_FAULT   : 1;
        uint8_t Reserved           : 1;
        uint8_t VBUS_DISCH_FAULT   : 1;
        uint8_t VPU_PRESENCE       : 1;
        uint8_t VPU_OVP_FAULT      : 1;
    } b;
} STUSB_GEN1S_HW_FAULT_STATUS_RegTypeDef;

/************************************************************************************************************
  * @brief:   STUSB_GEN1S PD_TYPEC_STATUS register Structure definition
  * @Address:   14h
  * @Access:  RO
  * @Note:  This register provides handcheck from typeC
  ************************************************************************************************************/
#define PD_TYPEC_STATUS 0x14
typedef union
{
    uint8_t d8;
    struct
    {
        uint8_t PD_TYPEC_HAND_CHECK : 4;
        uint8_t Reserved_4_7        : 4;
    } b;
} STUSB_GEN1S_PD_TYPEC_STATUS_RegTypeDef;

/************************************************************************************************************
  * @brief:   STUSB_GEN1S TYPE_C_STATUS register Structure definition
  * @Address:   15h
  * @Access:  RO
  * @Note:  This register provides information on typeC connection status
  ************************************************************************************************************/
#define REG_TYPE_C_STATUS 0x15
typedef union
{
    uint8_t d8;
    struct
    {
        uint8_t TYPEC_FSM_STATE : 5;
        uint8_t PD_SNK_TX_RP    : 1;
        uint8_t PD_SRC_TX_RP    : 1;
        uint8_t REVERSE         : 1;
    } b;
} STUSB_GEN1S_TYPE_C_STATUS_RegTypeDef;

#define MASK_REVERSE 0x80 //0: CC1 is attached 1: CC2 is Attach.

/************************************************************************************************************
  * @brief:   STUSB_GEN1S PRT_STATUS register Structure definition
  * @Address:   16h
  * @Access:  RO
  * @Note:  This register provides information on PRT status
  *
  ************************************************************************************************************/
#define PRT_STATUS 0x16
typedef union
{
    uint8_t d8;
    struct
    {
        uint8_t HWRESET_RECEIVED : 1;
        uint8_t HWRESET_DONE     : 1;
        uint8_t MSG_RECEIVED     : 1;
        uint8_t MSG_SENT         : 1;
        uint8_t BIST_RECEIVED    : 1;
        uint8_t BIST_SENT        : 1;
        uint8_t Reserved_6       : 1;
        uint8_t TX_ERROR         : 1;
    } b;
} STUSB_GEN1S_PRT_STATUS_RegTypeDef;

/************************************************************************************************************
  * @brief:   STUSB_GEN1S PHY_STATUS register Structure definition
  * @Address:   17h
  * @Access:  RO
  * @Note:  This register provides information on PHY status
  *
  ************************************************************************************************************/
#define PHY_STATUS 0x17
typedef union
{
    uint8_t d8;
    struct
    {
        uint8_t TX_MSG_FAIL : 1;
        uint8_t TX_MSG_DISC : 1;
        uint8_t TX_MSG_SUCC : 1;
        uint8_t IDLE        : 1;
        uint8_t Reserved2   : 1;
        uint8_t SOP_RX_Type : 3;

    } b;
} STUSB_GEN1S_PHY_STATUS_RegTypeDef;

/************************************************************************************************************
  * @brief:   STUSB_GEN1S CC_CAPABILITY_CTRL register Structure definition
  * @Address:   18h
  * @Access:  R/W
  * @Note:  This register allows to change the advertising of the current capability and the VCONN
  *   supply capability when operating in Source power role.
  ************************************************************************************************************/
#define CC_CAPABILITY_CTRL 0x18
typedef union
{
    uint8_t d8;
    struct
    {
        uint8_t CC_VCONN_SUPPLY_EN    : 1;
        uint8_t VCONN_SWAP_EN         : 1;
        uint8_t PR_SWAP_EN            : 1;
        uint8_t DR_SWAP_EN            : 1;
        uint8_t CC_VCONN_DISCHARGE_EN : 1;
        uint8_t SNK_DISCONNECT_MODE   : 1;
        uint8_t CC_CURRENT_ADVERTISED : 2;
    } b;
} STUSB_GEN1S_CC_CAPABILITY_CTRL_RegTypeDef;

/************************************************************************************************************
  * @brief:   STUSB_GEN1S PRT_TX_CTRL register Structure definition
  * @Address:   19h
  * @Access:  R/W
  * @Note:  This register allows PRL TX layer
  ************************************************************************************************************/
#define STUSB_GEN1S_PRT_TX_CTRL 0x19
typedef union
{
    uint8_t d8;
    struct
    {
        uint8_t PRT_TX_SOP_MSG    : 3;
        uint8_t reserved_3        : 1;
        uint8_t PRT_RETRY_MSG_CNT : 2;
        uint8_t reserved_6_7      : 2;
    } b;
} STUSB_GEN1S_PRT_TX_CTRL_RegTypeDef;

/************************************************************************************************************
  * @brief:   STUSB_GEN1S PD_COMMAND_CTRL register Structure definition
  * @Address:   1Ah
  * @Access:  R/W
  * @Note:  This register allows to send command to PRL or PE internal state machines
  ************************************************************************************************************/
#define STUSB_GEN1S_CMD_CTRL 0x1A
typedef union
{
    uint8_t d8;
    struct
    {
        uint8_t PD_CMD : 8;
    } b;
} STUSB_GEN1S_PD_CMD_CTRL_RegTypeDef;

/************************************************************************************************************
  * @brief:   STUSB_GEN1S_DEVICE_CTRL register Structure definition
  * @Address:   1Dh
  * @Access:  R/W
  * @Note:  This register allows to Reset PHYTX & change device Automation level
  ************************************************************************************************************/
#define STUSB_GEN1S_DEV_CTRL 0x1D
typedef union
{
    uint8_t d8;
    struct
    {
        uint8_t reserved_0   : 1;
        uint8_t PHY_TX_RESET : 1;
        uint8_t reserved_2_5 : 4;
        uint8_t PD_TOP_LAYER : 2;
    } b;
} STUSB_GEN1S_DEVICE_CTRL_RegTypeDef;

#define SNK_UNATTACHED        0x00
#define SNK_ATTACHWAIT        0x01
#define SNK_ATTACHED          0x02
#define SNK_2_SRC_PR_SWAP     0x06
#define SNK_TRYWAIT           0x07
#define SRC_UNATTACHED        0x08
#define SRC_ATTACHWAIT        0x09
#define SRC_ATTACHED          0x0A
#define SRC_2_SNK_PR_SWAP     0x0B
#define SRC_TRY               0x0C
#define ACCESSORY_UNATTACHED  0x0D
#define ACCESSORY_ATTACHWAIT  0x0E
#define ACCESSORY_AUDIO       0x0F
#define ACCESSORY_DEBUG       0x10
#define ACCESSORY_POWERED     0x11
#define ACCESSORY_UNSUPPORTED 0x12
#define ERRORRECOVERY         0x13

#define STUSB_GEN1S_MONITORING_CTRL_0 0x20
#define STUSB_GEN1S_MONITORING_CTRL_1 0x21
#define STUSB_GEN1S_MONITORING_CTRL_2 0x22

typedef union
{
    uint8_t d8;
    struct
    {
        uint8_t VSEL_PDO  : 8;
    } b;
} STUSB_GEN1S_MONITORING_CTRL_1_RegTypeDef;

/************************************************************************************************************
  * @brief:   STUSB RESET_CTRL register Structure definition
  * @Address:   23h
  * @Access:  R/W
  * @Note:  This register allows to reset the device by software.
  *   The SW_RESET_EN bit acts as the hardware RESET pin but it does not command the RESET pin.
  ************************************************************************************************************/
#define STUSB_GEN1S_RESET_CTRL_REG 0x23
typedef union
{
    uint8_t d8;
    struct
    {
        uint8_t SW_RESET_EN   : 1;
        uint8_t _Reserved_1_7 : 7;
    } b;
} STUSB_GEN1S_RESET_CTRL_RegTypeDef;

typedef enum
{
    No_SW_RST = 0, /*!< DEFAULT: Device reset is performed through the hardware RESET pin */
    SW_RST    = 1  /*!< Force the device reset as long as this bit value is set */
} SW_RESET_TypeDef;

#define PE_FSM 0x29
/* PE STATES */
#define PE_SOFT_RESET                        0x01          //     "00000001"
#define PE_HARD_RESET                        0x02           //     "00000010"
#define PE_SEND_SOFT_RESET                   0x03           //     "00000011"
#define PE_BIST_CARRIER_MODE                 0x04           //     "00000100"

#define PE_SNK_STARTUP                       0x12           //     "00010010"
#define PE_SNK_DISCOVERY                     0x13           //     "00010011"
#define PE_SNK_WAIT_FOR_CAPABILITIES         0x14           //     "00010100"
#define PE_SNK_EVALUATE_CAPABILITIES         0x15           //     "00010101"
#define PE_SNK_SELECT_CAPABILITIES           0x16           //     "00010110"
#define PE_SNK_TRANSITION_SINK               0x17           //     "00010111"
#define PE_SNK_READY                         0x18           //     "00011000"
#define PE_SNK_READY_SENDING                 0x19           //     "00011001"
#define PE_DB_CP_CHECK_FOR_VBUS              0x1A           //     "00011010"
// 0x19 -- 0x29 reserved
#define PE_ERRORRECOVERY                     0x30           //     "00110000"
#define PE_SRC_TRANSITION_SUPPLY_3           0x31           //     "00110001"
#define PE_SRC_TRANSITION_SUPPLY_2B          0x31           //     "00110001"
#define PE_SRC_GET_SINK_CAP                  0x31           //

#define RX_BYTE_CNT  0x30
#define RX_HEADER    0x31 // RX message header (16bit)
#define RX_DATA_OBJ  0x33
#define RX_DATA_OBJ1 0x33 // (32bit)
#define RX_DATA_OBJ2 0x37 // (32bit)
#define RX_DATA_OBJ3 0x3B // (32bit)
#define RX_DATA_OBJ4 0x3F // (32bit)
#define RX_DATA_OBJ5 0x33 // (32bit)
#define RX_DATA_OBJ6 0x37 // (32bit)
#define RX_DATA_OBJ7 0x4B // (32bit)

#define TX_BYTE_CNT    0x50
#define TX_HEADER      0x51 // (16bit)
#define TX_HEADER_LOW  0x51
#define TX_HEADER_HIGH 0x52
#define TX_DATA_OBJ1   0x53
#define TX_DATA_OBJ2   0x57
#define TX_DATA_OBJ3   0x5B
#define TX_DATA_OBJ4   0x5F
#define TX_DATA_OBJ5   0x63
#define TX_DATA_OBJ6   0x67
#define TX_DATA_OBJ7   0x6B

#define DPM_PDO_NUMB 0x70

#define DPM_SNK_PDO1 0x85
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t Operational_Current        : 10;
        uint32_t Voltage                    : 10;
        uint8_t  Reserved_22_20             :  3;
        uint8_t  Fast_Role_Req_cur          :  2;  /* must be set to 0 in 2.0 */
        uint8_t  Dual_Role_Data             :  1;
        uint8_t  USB_Communications_Capable :  1;
        uint8_t  Unconstrained_Power        :  1;
        uint8_t  Higher_Capability          :  1;
        uint8_t  Dual_Role_Power            :  1;
        uint8_t  Fixed_Supply               :  2;
    } fix;
    struct
    {
        uint32_t Operating_Current : 10;
        uint32_t Min_Voltage       : 10;
        uint32_t Max_Voltage       : 10;
        uint8_t  VariableSupply    :  2;
    } var;
    struct
    {
        uint32_t Operating_Power : 10;
        uint32_t Min_Voltage     : 10;
        uint32_t Max_Voltage     : 10;
        uint8_t  Battery         :  2;
    } bat;

} USB_PD_SNK_PDO_TypeDef;

#define PDO_SNK_FIX_FIXED    0
#define PDO_SNK_FIX_VARIABLE 1
#define PDO_SNK_FIX_BATTERY  2

typedef union
{
    uint32_t d32;
    struct
    {
        //Table 6-9 Fixed Supply PDO - Source
        uint32_t Max_Operating_Current : 10; // bits 9..0
        uint32_t Voltage               : 10; // bits 19..10
        uint8_t  PeakCurrent           :  2; // bits 21..20
        uint8_t  Reserved              :  3; //
        uint8_t  DataRoleSwap          :  1; // bits 25
        uint8_t  Communication         :  1; // bits 26
        uint8_t  ExternalyPowered      :  1; // bits 27
        uint8_t  SuspendSuported       :  1; // bits 28
        uint8_t  DualRolePower         :  1; // bits 29
        uint8_t  FixedSupply           :  2; // bits 31..30
    } fix;
    struct
    {
        //Table 6-11 Variable Supply (non-Battery) PDO - Source
        uint32_t Operating_Current : 10;
        uint32_t Min_Voltage       : 10;
        uint32_t Max_Voltage       : 10;
        uint8_t  VariableSupply    :  2;
    } var;
    struct
    {
        //Table 6-12 Battery Supply PDO - Source
        uint32_t Operating_Power : 10;
        uint32_t Min_Voltage     : 10;
        uint32_t Max_Voltage     : 10;
        uint8_t  Battery         :  2;
    } bat;

} USB_PD_SRC_PDO_TypeDef;

#define DPM_REQ_RDO    0x91 // (32bits)
#define RDO_REG_STATUS 0x91
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t MaxCurrent        : 10; // Bits 9..0
        uint32_t OperatingCurrent  : 10;
        uint8_t  reserved_22_20    :  3;
        uint8_t  UnchunkedMess_sup :  1;
        uint8_t  UsbSuspend        :  1;
        uint8_t  UsbComCap         :  1;
        uint8_t  CapaMismatch      :  1;
        uint8_t  GiveBack          :  1;
        uint8_t  Object_Pos        :  3; // Bits 30..28 (3-bit)
        uint8_t  reserved_31       :  1; // Bits 31

    } b;
} STUSB_GEN1S_RDO_REG_STATUS_RegTypeDef;

//Table 6-5 Control Message Types
#define USBPD_CTRLMSG_Reserved1                 0x00
#define USBPD_CTRLMSG_GoodCRC                   0x01
#define USBPD_CTRLMSG_GotoMin                   0x02
#define USBPD_CTRLMSG_Accept                    0x03
#define USBPD_CTRLMSG_Reject                    0x04
#define USBPD_CTRLMSG_Ping                      0x05
#define USBPD_CTRLMSG_PS_RDY                    0x06
#define USBPD_CTRLMSG_Get_Source_Cap            0x07
#define USBPD_CTRLMSG_Get_Sink_Cap              0x08
#define USBPD_CTRLMSG_DR_Swap                   0x09
#define USBPD_CTRLMSG_PR_Swap                   0x0A
#define USBPD_CTRLMSG_VCONN_Swap                0x0B
#define USBPD_CTRLMSG_Wait                      0x0C
#define USBPD_CTRLMSG_Soft_Reset                0x0D
#define USBPD_CTRLMSG_Reserved2                 0x0E
#define USBPD_CTRLMSG_Reserved3                 0x0F
#define USBPD_CTRLMSG_Not_Supported             0x10
#define USBPD_CTRLMSG_Get_Source_Cap_Extended   0x11
#define USBPD_CTRLMSG_Get_Status                0x12
#define USBPD_CTRLMSG_FR_Swap                   0x13
#define USBPD_CTRLMSG_Get_PPS_Status            0x14
#define USBPD_CTRLMSG_Get_Country_Codes         0x15
#define USBPD_CTRLMSG_Reserved4                 0x16
#define USBPD_CTRLMSG_Reserved5                 0x1F

//Table 6-6 Data Message Types
#define USBPD_DATAMSG_Reserved1             0x00
#define USBPD_DATAMSG_Source_Capabilities   0x01
#define USBPD_DATAMSG_Request               0x02
#define USBPD_DATAMSG_BIST                  0x03
#define USBPD_DATAMSG_Sink_Capabilities     0x04
#define USBPD_DATAMSG_Battery_Status        0x05
#define USBPD_DATAMSG_Alert                 0x06
#define USBPD_DATAMSG_Get_Country_Info      0x07
#define USBPD_DATAMSG_Reserved2             0x08
#define USBPD_DATAMSG_Reserved3             0x0E
#define USBPD_DATAMSG_Vendor_Defined        0x0F
#define USBPD_DATAMSG_Reserved4             0x10
#define USBPD_DATAMSG_Reserved5             0x1F

/* Identification of STUSB */
#define REG_DEVICE_ID (uint8_t)0x2F
#define EXTENTION_10  (uint8_t)0x80
#define ID_Reg        (uint8_t)0x1C
#define CUT           (uint8_t)3 << 2
#define CUT_A         (uint8_t)4 << 2
#define GEN1S         (uint8_t)8 << 2
#define DEV_CUT       (uint8_t)0x03

/* NVM FLasher Registers Definition */
#define FTP_CUST_PASSWORD_REG 0x95
#define FTP_CUST_PASSWORD     0x47
#define FTP_CTRL_0            0x96
#define FTP_CUST_PWR          0x80
#define FTP_CUST_RST_N        0x40
#define FTP_CUST_REQ          0x10
#define FTP_CUST_SECT         0x07
#define FTP_CTRL_1            0x97
#define FTP_CUST_SER          0xF8
#define FTP_CUST_OPCODE       0x07
#define RW_BUFFER             0x53

/*
    "000" then No Operation
    "001" then Read
    "010" and FTP_ADR[2:0]="000" then Shift-In Write Bit Data (0x20-0x28). (to be done before Programming)
    "010" and FTP_ADR[2:0]="001" then Shift-In Erase Sector Data (0x20). (to be done before Erasing)
    "011" and FTP_ADR[2:0]="000" then Shift-Out Read Bit Data (0x20-0x28). (to be done after Reading)
    "011" and FTP_ADR[2:0]="001" then Shift-Out Erase Sector Data (0x20). (to be done after Erasing)
    "100" then Verify (to be done after Programming)
    "101" then Erase
    "110" then Program
    "111" then Soft Programming (to be done after Erasing)
*/
#define READ             0x00
#define WRITE_PL         0x01
#define WRITE_SER        0x02
#define READ_PL          0x03
#define READ_SER         0x04
#define ERASE_SECTOR     0x05
#define PROG_SECTOR      0x06
#define SOFT_PROG_SECTOR 0x07

#define SECTOR_0  0x01
#define SECTOR_1  0x02
#define SECTOR_2  0x04
#define SECTOR_3  0x08
#define SECTOR_4  0x10
#define SECTOR_5  0x20

#ifdef __cplusplus
}
#endif

#endif /* __LIBSTUSB4500_REGISTERS_H */

// ------------------------------------------------------------------ defines --

#define VERSION_MAJ 1
#define VERSION_MIN 0
#define VERSION_REV 5

#define STUSB4500_I2C_SLAVE_BASE_ADDR 0x28

#define USBPD_REV_3_0_SUPPORT      1
#define USBPD_MESSAGE_QUEUE_SZ    32
#define USBPD_INTERRUPT_QUEUE_SZ  32
#define NVM_SNK_PDO_MAX            3
#define NVM_SRC_PDO_MAX           10
#define DEFAULT_SRC_CAP_REQ_MAX  200

// ------------------------------------------------------------------- macros --

/* nothing */

// ----------------------------------------------------------- exported types --

class STUSB4500Version {
protected:
  constexpr static uint8_t const _strSize = 12U;
  char _str[_strSize] = { '\0' };
  uint8_t const _major;
  uint8_t const _minor;
  uint8_t const _revision;
  uint32_t const _base256;

public:
  STUSB4500Version(
      uint8_t const major,
      uint8_t const minor,
      uint8_t const revision
  ):
    _major(major),
    _minor(minor),
    _revision(revision),
    _base256(((major << 16U) | (minor << 8U) | revision) & 0x00FFFFFF)
  {
    sprintf(_str, "%u.%u.%u", _major, _minor, _revision);
  }
  char const *str() const
    { return _str; }
  inline bool operator==(STUSB4500Version const &version)
    { return _base256 == version._base256; }
  inline bool operator!=(STUSB4500Version const &version)
    { return _base256 != version._base256; }
  inline bool operator<(STUSB4500Version const &version)
    { return _base256 < version._base256; }
};

enum class CableStatus {
  NONE = -1,
  NotConnected, // = 0
  CC1Connected, // = 1
  CC2Connected, // = 2
  COUNT         // = 3
};

enum class PDOSupplyType {
  NONE = -1,
  Fixed,    // = 0
  Variable, // = 1
  Battery,  // = 2
  COUNT     // = 3
};

class PDO {
public:
  size_t   number;
  uint16_t voltage_mV;
  uint16_t current_mA;
  uint16_t maxCurrent_mA;
  PDO(void):
    number(0U),
    voltage_mV(0U),
    current_mA(0U),
    maxCurrent_mA(0U)
  {}
  PDO(
      size_t const number,
      uint16_t const voltage_mV,
      uint16_t const current_mA,
      uint16_t const maxCurrent_mA = 0U
  ):
    number(number),
    voltage_mV(voltage_mV),
    current_mA(current_mA),
    maxCurrent_mA(maxCurrent_mA)
  {}

  inline bool operator==(PDO const &pdo)
  {
    return (    voltage_mV == pdo.voltage_mV    ) &&
           (    current_mA == pdo.current_mA    ) &&
           ( maxCurrent_mA == pdo.maxCurrent_mA ) ;
  }
  inline bool operator!=(PDO const &pdo)
  {
    return (    voltage_mV != pdo.voltage_mV    ) ||
           (    current_mA != pdo.current_mA    ) ||
           ( maxCurrent_mA != pdo.maxCurrent_mA ) ;
  }
};

typedef void (* USBEventCallback)(void);

class USBPDStatus {
public:
  uint8_t                                    hwReset;
  STUSB_GEN1S_HW_FAULT_STATUS_RegTypeDef     hwFaultStatus;     // 8-bit
  STUSB_GEN1S_MONITORING_STATUS_RegTypeDef   monitoringStatus;  // 8-bit
  STUSB_GEN1S_CC_DETECTION_STATUS_RegTypeDef ccDetectionStatus; // 8-bit
  STUSB_GEN1S_CC_STATUS_RegTypeDef           ccStatus;          // 8-bit
  STUSB_GEN1S_PRT_STATUS_RegTypeDef          prtStatus;         // 8-bit
  STUSB_GEN1S_PHY_STATUS_RegTypeDef          phyStatus;         // 8-bit
  STUSB_GEN1S_RDO_REG_STATUS_RegTypeDef      rdoSnk;
  size_t                                     pdoSnkCount;
  USB_PD_SNK_PDO_TypeDef                     pdoSnk[NVM_SNK_PDO_MAX];
  size_t                                     pdoSrcCount;
  USB_PD_SRC_PDO_TypeDef                     pdoSrc[NVM_SRC_PDO_MAX];

  USBPDStatus(void)
  {
    hwReset              = 0U;
    hwFaultStatus.d8     = 0U;
    monitoringStatus.d8  = 0U;
    ccDetectionStatus.d8 = 0U;
    ccStatus.d8          = 0U;
    prtStatus.d8         = 0U;
    phyStatus.d8         = 0U;
    rdoSnk.d32           = 0U;

    pdoSnkCount = 0U;
    for (size_t i = 0U; i < NVM_SNK_PDO_MAX; ++i)
      { pdoSnk[i].d32 = 0U; }

    pdoSrcCount = 0U;
    for (size_t i = 0U; i < NVM_SRC_PDO_MAX; ++i)
      { pdoSrc[i].d32 = 0U; }
  }
};

class USBPDStateMachine {
public:
  volatile uint8_t alertReceived;
  volatile uint8_t attachReceived;
  uint16_t irqReceived;
  uint16_t irqHardReset;
  uint16_t attachTransition;
  uint16_t srcPDOReceived;
  uint16_t srcPDORequesting;
  uint16_t psrdyReceived;
  uint16_t msgReceived;
  uint16_t msgAccept;
  uint16_t msgReject;
  uint16_t msgGoodCRC;
  uint8_t  msgHead;
  uint8_t  msgTail;
  uint8_t  msg[USBPD_MESSAGE_QUEUE_SZ];
  uint8_t  irqHead;
  uint8_t  irqTail;
  uint8_t  irq[USBPD_INTERRUPT_QUEUE_SZ];

  USBPDStateMachine(void)
  {
    alertReceived    = 0U;
    attachReceived   = 0U;
    irqReceived      = 0U;
    irqHardReset     = 0U;
    attachTransition = 0U;
    srcPDOReceived   = 0U;
    srcPDORequesting = 0U;
    psrdyReceived    = 0U;
    msgReceived      = 0U;
    msgAccept        = 0U;
    msgReject        = 0U;
    msgGoodCRC       = 0U;

    msgHead = 0U;
    msgTail = 0U;
    for (size_t i = 0U; i < USBPD_MESSAGE_QUEUE_SZ; ++i)
      { msg[i] = 0U; }

    irqHead = 0U;
    irqTail = 0U;
    for (size_t i = 0U; i < USBPD_INTERRUPT_QUEUE_SZ; ++i)
      { irq[i] = 0U; }
  }
};

class STUSB4500 {
protected:
  STUSB4500Version _VERSION =
      STUSB4500Version(VERSION_MAJ, VERSION_MIN, VERSION_REV);

  uint16_t       _resetPin;
  uint8_t        _slaveAddress; // real address, NOT shifted
  TwoWire const *_wire;

  uint16_t _srcCapRequestMax;

  USBPDStatus       _status;
  USBPDStateMachine _state;

  PDO _snkRDO;
  PDO _snkPDO[NVM_SNK_PDO_MAX];
  PDO _srcPDO[NVM_SRC_PDO_MAX];

  USBEventCallback _cableAttached;
  USBEventCallback _cableDetached;
  USBEventCallback _sourceCapabilitiesReceived;

private:
  // whether or not the device has been initialized
  bool _started;

  // I2C read/write operations
  bool wireRead(
      uint16_t const addr, uint8_t *buff, uint16_t const size) const;
  bool wireWrite(
      uint16_t const addr, uint8_t *buff, uint16_t const size) const;

  // STUSB4500 device operations
  bool clearAlerts(bool const unmask);
  bool updatePrtStatus(void);

  bool updateRDOSnk(void);
  bool updatePDOSnk(void);
  bool setPDOSnkCount(uint8_t const count);
  bool setPDOSnk(PDO const pdo);
  bool clearPDOSrc(void);
  bool updatePDOSrc(void);

  bool sendPDCableReset(void);

  void processAlerts(void);
  void processAttach(void);

  bool pushPDMessage(uint8_t const type, uint8_t const value);
  bool popPDMessage(uint8_t &type);
  bool pushPDInterrupt(uint8_t const type);
  bool popPDInterrupt(uint8_t &type);

public:
  STUSB4500(
      uint16_t const resetPin,
      uint8_t const slaveAddress,
      TwoWire const *wire
  );
  STUSB4500(uint16_t const resetPin);

  char const *version() { return _VERSION.str(); }

  // ready the object for first use, verify I2C comms
  bool started(void) const { return _started; }
  bool begin(uint16_t const alertPin, uint16_t const attachPin);
  bool initialize(void); // (re-)initialize the device, registers, and flags

  bool ready(void) const; // check if the device is responding on I2C interface
  void waitUntilReady(void) const; // block until ready

  // (de-)assert the hardware reset pin to enable or disable power output
  void enablePower(bool const enable, bool const wait);

  void reset(bool const wait);     // hard reset using device pin
  void softReset(bool const wait); // soft reset using device register

  void update(void); // main event loop, call as frequently as possible
  void alertISR(void);
  void attachISR(void);

  CableStatus cableStatus(void) const; // check if cable attached/orientation

  void setMaxSourceCapabilityRequests(uint16_t const max)
    { _srcCapRequestMax = max; }

  bool requestSourceCapabilities(void); // request source caps from source
  bool updateSinkCapabilities(void); // retrieve sink caps from device

  size_t sourcePDOCount(void) const { return _status.pdoSrcCount; }
  PDO sourcePDO(size_t const n) const; // currently stored src PDO at index n

  size_t sinkPDOCount(void) const { return _status.pdoSnkCount; }
  PDO sinkPDO(size_t const n) const; // currently stored snk PDO at index n

  PDO requestedPDO(void); // currently requested PDO (RDO)

  // immediately set power to fixed PDO with given voltage and current
  bool setPower(uint32_t const voltage_mV, uint32_t const current_mA);
  bool setPowerDefaultUSB(void); // set fixed PDO to 5V 1.5A

  // callback registration
  void setCableAttached(USBEventCallback const callback)
    { _cableAttached = callback; }
  void setCableDetached(USBEventCallback const callback)
    { _cableDetached = callback; }
  void setSourceCapabilitiesReceived(USBEventCallback const callback)
    { _sourceCapabilitiesReceived = callback; }
};

// ------------------------------------------------------- exported variables --

/* nothing */

// ------------------------------------------------------- exported functions --



//#define I2C_READ_TIMEOUT_MS    2000
//#define I2C_WRITE_TIMEOUT_MS   2000
#define I2C_CLOCK_FREQ_HZ    400000

#define EVAL_DEVICE_ID         0x21 // in device ID reg (0x2F)
#define PROD_DEVICE_ID         0x25 //

#define TLOAD_REG_INIT_MS       250 // section 6.1.3 in datasheet
#define SW_RESET_DEBOUNCE_MS     27
#define ATTACH_DEBOUNCE_MS       25

#define USB_DEFAULT_VOLTAGE_MV 5000
#define USB_DEFAULT_CURRENT_MA 1500

#define USBPD_MESSAGE_TYPE_DATA 'D'
#define USBPD_MESSAGE_TYPE_CTRL 'C'

// ----------------------------------------------------------- private macros --

// convert value at addr to little-endian (16-bit)
#define LE_u16(addr)                                      \
    ( ( (((uint16_t)(*(((uint8_t *)(addr)) + 1)))     ) + \
        (((uint16_t)(*(((uint8_t *)(addr)) + 0))) << 8) ) )

// convert value at addr to little-endian (32-bit)
#define LE_u32(addr)                                       \
    ( ( (((uint32_t)(*(((uint8_t *)(addr)) + 3)))      ) + \
        (((uint32_t)(*(((uint8_t *)(addr)) + 2))) <<  8) + \
        (((uint32_t)(*(((uint8_t *)(addr)) + 1))) << 16) + \
        (((uint32_t)(*(((uint8_t *)(addr)) + 0))) << 24) ) )

#define NO_INTERRUPT(e) { noInterrupts(); (e); interrupts(); }
#define DELAY(ms) delay(ms)

#define CABLE_CONNECTED(c) \
    ((CableStatus::CC1Connected == (c)) || (CableStatus::CC2Connected == (c)))

// ------------------------------------------------------------ private types --

typedef union
{
    uint16_t d16;
    struct {
#if defined(USBPD_REV_3_0_SUPPORT)
        uint16_t messageType     : 5; // USBPD rev >= 3.0 message type
#else
        uint16_t messageType     : 4; // USBPD rev  < 3.0 message type
    uint16_t reserved_4      : 1; // reserved
#endif
        uint16_t portDataRole    : 1; // port data role
        uint16_t specRevision    : 2; // spec revision
        uint16_t portPowerRole   : 1; // port power role/cable plug
        uint16_t messageID       : 3; // message ID
        uint16_t dataObjectCount : 3; // number of data objects
        uint16_t extended        : 1; // reserved
    } b;
}
        USBPDMessageHeader;

// ------------------------------------------------------- exported variables --

static STUSB4500 *_usbpd; // used for reference in ISRs

// -------------------------------------------------------- private variables --

/* nothing */

// -------------------------------------------------------- private functions --

static void _alertISR(void) { _usbpd->alertISR(); }
static void _attachISR(void) { _usbpd->attachISR(); }

// ------------------------------------------------------- exported functions --

STUSB4500::STUSB4500(
        uint16_t const resetPin,
        uint8_t const slaveAddress,
        TwoWire const *wire
):
        _resetPin(resetPin),
        _slaveAddress(slaveAddress),
        _wire(wire),
        _srcCapRequestMax(DEFAULT_SRC_CAP_REQ_MAX),
        _status(),
        _state()
{
    _snkRDO = PDO();
    for (size_t i = 0U; i < NVM_SNK_PDO_MAX; ++i)
    { _snkPDO[i] = PDO(); }
    for (size_t i = 0U; i < NVM_SRC_PDO_MAX; ++i)
    { _srcPDO[i] = PDO(); }

    _cableAttached = nullptr;
    _cableDetached = nullptr;
    _sourceCapabilitiesReceived = nullptr;

    _usbpd = nullptr; // NULL until begin() is called with ALRT/ATCH pins
}

STUSB4500::STUSB4500(
        uint16_t const resetPin
):
        STUSB4500(
                resetPin, STUSB4500_I2C_SLAVE_BASE_ADDR, &Wire)
{
    /* empty */
}

bool STUSB4500::begin(uint16_t const alertPin, uint16_t const attachPin)
{
    if (nullptr == _usbpd) {
        _usbpd = this;
        attachInterrupt(digitalPinToInterrupt(alertPin), _alertISR, FALLING);
        attachInterrupt(digitalPinToInterrupt(attachPin), _attachISR, CHANGE);

        // initialize I2C interface
        TwoWire *wire = (TwoWire *)_wire;
//        wire->begin();
//        wire->setClock(I2C_CLOCK_FREQ_HZ);
    }

    _started = false;
    if (ready())
    { _started = initialize(); }

    return _started;
}

/****
 * initializes the STUSB4500 device by waiting until it is responding to I2C
 * commands, clearing any current alerts, and unmasking useful interrupts.
 ****/
bool STUSB4500::initialize(void)
{
    if (!setPDOSnkCount(1U)) { return false; }
    if (!updatePDOSnk()) { return false; }
    if (!updateRDOSnk()) { return false; }
    if (!clearAlerts(true)) { return false; }
    if (!updatePrtStatus()) { return false; }
    if (!clearPDOSrc()) { return false; }

    CableStatus cable = cableStatus();
    if (CABLE_CONNECTED(cable)) {
        if (!updatePDOSrc())
        { return false; }
    }

    return true;
}

/****
 * query the internal device ID register of the STUSB4500 and verify it matches
 * the expected manufacturer-specified ID. this is used to determine if the
 * device has powered on and can respond to I2C read requests.
 ****/
bool STUSB4500::ready(void) const
{
    uint8_t deviceID;

    if (!wireRead(REG_DEVICE_ID, &deviceID, 1U)) { return false; }

    return (EVAL_DEVICE_ID == deviceID) || (PROD_DEVICE_ID == deviceID);
}

/****
 * repeatedly query the device in blocking mode until it is ready.
 ****/
void STUSB4500::waitUntilReady(void) const
{
    while (!ready()) { continue ; }
}

/****
 * enable or disable power output by toggling the hardware reset pin. this
 * routine can optionally block return until the device has verifiably powered
 * back on and is responding to I2C commands.
 ****/
void STUSB4500::enablePower(bool const enable, bool const wait)
{
    // uses hardware reset pin to toggle power output
    if (enable) {
        digitalWrite(_resetPin, LOW);

        // wait for I2C registers to be initialized from NVM
        DELAY(TLOAD_REG_INIT_MS);

        if (wait)
        { waitUntilReady(); }
    }
    else {
        digitalWrite(_resetPin, HIGH);
        DELAY(25);
    }
}

/****
 * assert and de-assert the hardware reset pin on the STUSB4500. after reset,
 * the device powers on and behaves according to NVM settings by default.
 * this routine can optionally block return until the device has verifiably
 * powered back on and is responding to I2C commands.
 ****/
void STUSB4500::reset(bool const wait)
{
    // the reset pin on STUSB4500 is active high, so driving high temporarily
    // will reset the device
    digitalWrite(_resetPin, HIGH);
    DELAY(25);
    digitalWrite(_resetPin, LOW);

    // wait for I2C registers to be initialized from NVM
    DELAY(TLOAD_REG_INIT_MS);

    if (wait)
    { waitUntilReady(); }
}

/****
 * resets the STUSB4500 by temporarily setting and then clearing the reset
 * register. this resets the internal Type-C and USB PD state machines and
 * causes electrical disconnect on both source and sink sides. while reset, all
 * pending interrupt alarms are cleared. this routine can optionally block
 * return until the device has verifiably powered back on and is responding to
 * I2C commands.
 ****/
void STUSB4500::softReset(bool const wait)
{
    uint8_t buff = SW_RST;
    if (!wireWrite(STUSB_GEN1S_RESET_CTRL_REG, &buff, 1U))
    { return; }

    if (!clearAlerts(false))
    { return; }

    DELAY(SW_RESET_DEBOUNCE_MS);

    // restore reset control register
    buff = No_SW_RST;
    if (!wireWrite(STUSB_GEN1S_RESET_CTRL_REG, &buff, 1U))
    { return; }

    // wait for I2C registers to be initialized from NVM
    DELAY(TLOAD_REG_INIT_MS);

    if (wait)
    { waitUntilReady(); }
}

void STUSB4500::update(void)
{
    static CableStatus cablePrev = CableStatus::NONE;

    uint8_t irq;
    uint8_t msg;
    uint8_t value;

    while (0U != _state.alertReceived) {
        NO_INTERRUPT(--(_state.alertReceived));
        processAlerts();
    }

    while (0U != _state.irqReceived) {
        --(_state.irqReceived);
        popPDInterrupt(irq);
    }

    // process attach events first so that we don't do any unnecessary
    // processing until a cable is actually attached
    while (0U != _state.attachReceived) {
        NO_INTERRUPT(--(_state.attachReceived));
        processAttach();

        CableStatus cable = cableStatus();
        // ignore duplicate interrupts, only handle if the connection state
        // actually changed.
        if (cablePrev != cable) {
            if (CABLE_CONNECTED(cable)) {
                DELAY(ATTACH_DEBOUNCE_MS);
                if (NULL != _cableAttached)
                { _cableAttached(); }
            }
            else {
                // verify the new state isn't an error state -- that we have positively
                // determined the cable is -not- connected
                if (CableStatus::NotConnected == cable) {
                    if (NULL != _cableDetached)
                    { _cableDetached(); }
                }
                (void)clearPDOSrc();
            }
        }
        cablePrev = cable;
    }

    if (0U != _state.msgReceived) {
        --(_state.msgReceived);

        while (popPDMessage(msg)) {
            switch (msg) {
                case USBPD_MESSAGE_TYPE_CTRL:
                    if (popPDMessage(value)) { /* TBD */ }
                    break;
                case USBPD_MESSAGE_TYPE_DATA:
                    if (popPDMessage(value)) { /* TBD */ }
                    break;
                default:
                    break;
            }
        }
    }

    if (0U != _state.srcPDOReceived) {
        _state.srcPDOReceived = 0U;
        if (NULL != _sourceCapabilitiesReceived)
        { _sourceCapabilitiesReceived(); }
    }

    if (0U != _state.psrdyReceived) {
        --(_state.psrdyReceived);
    }
}

void STUSB4500::alertISR(void)
{
    ++(_state.alertReceived);
}

void STUSB4500::attachISR(void)
{
    ++(_state.attachReceived);
}

/****
 * read the port and Type-C status registers to determine if a USB cable is
 * connected to the STUSB4500 device. if connected, the orientation is
 * determined and indicated by return value of the selected CC line.
 ****/
CableStatus STUSB4500::cableStatus(void) const
{
    uint8_t portStatus;
    uint8_t typeCStatus;

    if (!wireRead(REG_PORT_STATUS, &portStatus, 1U))
    { return CableStatus::NONE; }

    if (VALUE_ATTACHED == (portStatus & STUSBMASK_ATTACHED_STATUS))
    {
        if (!wireRead(REG_TYPE_C_STATUS, &typeCStatus, 1U))
        { return CableStatus::NONE; }

        if (0U == (typeCStatus & MASK_REVERSE))
        { return CableStatus::CC1Connected; }
        else
        { return CableStatus::CC2Connected; }
    }
    else
    {
        return CableStatus::NotConnected;
    }
}

bool STUSB4500::requestSourceCapabilities(void)
{
    CableStatus cable = cableStatus();
    if (!CABLE_CONNECTED(cable)) { return false; }

    waitUntilReady();

    if (!clearAlerts(true)) { return false; }
    if (!updatePrtStatus()) { return false; }
    if (!clearPDOSrc()) { return false; }
    if (!updatePDOSrc()) { return false; }

    return true;
}

bool STUSB4500::updateSinkCapabilities(void)
{
    CableStatus cable = cableStatus();
    if (!CABLE_CONNECTED(cable)) { return false; }

    waitUntilReady();

    if (!updatePDOSnk()) { return false; }

    return true;
}

PDO STUSB4500::sourcePDO(size_t const n) const
{
    if (n < _status.pdoSrcCount)
    { return _srcPDO[n]; }

    return PDO(); // empty PDO
}

PDO STUSB4500::sinkPDO(size_t const n) const
{
    if (n < _status.pdoSnkCount)
    { return _snkPDO[n]; }

    return PDO(); // empty PDO
}

PDO STUSB4500::requestedPDO(void)
{
    PDO pdo = PDO();

    if (updateRDOSnk())
    { pdo = _snkRDO; }

    return pdo;
}

bool STUSB4500::setPower(uint32_t const voltage_mV, uint32_t const current_mA)
{
    CableStatus cable = cableStatus();
    if (!CABLE_CONNECTED(cable)) { return false; }

    if (!setPDOSnkCount(2U)) { return false; }
    if (!setPDOSnk(PDO(2U, voltage_mV, current_mA))) { return false; }
    if (!sendPDCableReset()) { return false; }
    if (!updatePDOSnk()) { return false; }

    return true;
}

bool STUSB4500::setPowerDefaultUSB(void)
{
    CableStatus cable = cableStatus();
    if (!CABLE_CONNECTED(cable)) { return false; }

    if (!setPDOSnkCount(1U)) { return false; }
    if (!sendPDCableReset()) { return false; }
    if (!updatePDOSnk()) { return false; }

    return true;
}

// -------------------------------------------------------- private functions --

/****
 * perform an I2C read on a STUSB4500 slave device.
 ****/
bool STUSB4500::wireRead(
        uint16_t const addr, uint8_t *buff, uint16_t const size) const
{
    uint8_t reg_addr = (uint8_t)(addr & 0xFF);
    uint8_t buff_sz = (uint8_t)(size & 0xFF);
    size_t count;

    TwoWire *wire = (TwoWire *)_wire;

    wire->beginTransmission(_slaveAddress);
    wire->write(reg_addr);  // set register for read
    wire->endTransmission(false); // false to not release the line
    wire->beginTransmission(_slaveAddress);
    wire->requestFrom(_slaveAddress, buff_sz);
    count = wire->readBytes(buff, buff_sz);
    if (count != size) { return false; }

    return true;
}

/****
 * perform an I2C write on a STUSB4500 slave device.
 ****/
bool STUSB4500::wireWrite(
        uint16_t const addr, uint8_t *buff, uint16_t const size) const
{
    uint8_t reg_addr = (uint8_t)(addr & 0xFF);
    uint8_t buff_sz  = (uint8_t)(size & 0xFF);
    uint8_t result;

    TwoWire *wire = (TwoWire *)_wire;

    wire->beginTransmission(_slaveAddress);
    wire->write(reg_addr); // command byte, sets register pointer address
    wire->write(buff, buff_sz);
    result = wire->endTransmission();
    if (result > 0U) { return false; }

    return true;
}

/****
 * clear all pending alerts by reading the status registers.
 ****/
bool STUSB4500::clearAlerts(bool const unmask)
{
    // clear alert status
    uint8_t alertStatus[12];
    if (!wireRead(ALERT_STATUS_1, alertStatus, 12U))
    { return false; }

    STUSB_GEN1S_ALERT_STATUS_MASK_RegTypeDef alertMask;
    if (unmask) {

        // set interrupts to unmask
        alertMask.d8 = 0xFF;

        //alertMask.b.PHY_STATUS_AL_MASK          = 0U;
        alertMask.b.PRT_STATUS_AL_MASK          = 0U;
        alertMask.b.PD_TYPEC_STATUS_AL_MASK     = 0U;
        //alertMask.b.HW_FAULT_STATUS_AL_MASK     = 0U;
        alertMask.b.MONITORING_STATUS_AL_MASK   = 0U;
        alertMask.b.CC_DETECTION_STATUS_AL_MASK = 0U;
        alertMask.b.HARD_RESET_AL_MASK          = 0U;

        // unmask the above alarms
        if (!wireWrite(ALERT_STATUS_MASK, &(alertMask.d8), 1U))
        { return false; }
    }

    return true;
}

bool STUSB4500::updatePrtStatus(void)
{
    uint8_t prtStatus[10];
    if (!wireRead(REG_PORT_STATUS, prtStatus, 10U))
    { return false; }

    _status.ccDetectionStatus.d8 = prtStatus[1];
    _status.ccStatus.d8          = prtStatus[3];
    _status.monitoringStatus.d8  = prtStatus[3];
    _status.hwFaultStatus.d8     = prtStatus[6];

    return true;
}

bool STUSB4500::updatePDOSnk(void)
{
#define BUFF_SZ NVM_SNK_PDO_MAX * sizeof(USB_PD_SNK_PDO_TypeDef)
    uint8_t pdo[BUFF_SZ];
    uint8_t pdoCount;

    waitUntilReady();

    if (!wireRead(DPM_PDO_NUMB, &pdoCount, 1U))
    { return false; }

    if (!wireRead(DPM_SNK_PDO1, pdo, BUFF_SZ))
    { return false; }

    _status.pdoSnkCount = pdoCount;
    for (uint8_t i = 0, j = 0; i < NVM_SNK_PDO_MAX; ++i, j += 4) {
        if (i < pdoCount) {
            _status.pdoSnk[i].d32 = LE_u32(&pdo[j]);
            _snkPDO[i] = PDO(i + 1,
                             _status.pdoSnk[i].fix.Voltage * 50U,
                             _status.pdoSnk[i].fix.Operational_Current * 10U);
        }
        else {
            _status.pdoSnk[i].d32 = 0U;
            _snkPDO[i] = PDO();
        }
    }
    return true;
#undef BUFF_SZ
}

bool STUSB4500::setPDOSnkCount(uint8_t const count)
{
    if ((count < 1) || (count > NVM_SNK_PDO_MAX))
    { return false; }

    uint8_t pdoCount = (uint8_t)count;

    return wireWrite(DPM_PDO_NUMB, &pdoCount, 1);
}

bool STUSB4500::setPDOSnk(PDO const pdo)
{
    if ((pdo.number < 1) || (pdo.number > NVM_SNK_PDO_MAX))
    { return false; }

    uint16_t voltage_mV = pdo.voltage_mV;
//  if (1U == pdo.number) // PDO 1 must always be USB +5V
//    { voltage_mV = 5000U; }

    // use the received PDO definition #1 (USB default) as template for new PDO,
    // just update the voltage and current to the input PDO.
    USB_PD_SNK_PDO_TypeDef def = _status.pdoSnk[0];
    uint8_t address = DPM_SNK_PDO1 + (4U * (pdo.number - 1U));
    def.fix.Voltage = voltage_mV / 50U;
    def.fix.Operational_Current = pdo.current_mA / 10U;

    return wireWrite(address, (uint8_t *)&(def.d32), 4U);
}

// static bool stusb4500_set_snk_var_pdo(void)
// {
//   return true;
// }

// static bool stusb4500_set_snk_bat_pdo(void)
// {
//   return true;
// }

bool STUSB4500::clearPDOSrc(void)
{
    _status.pdoSrcCount = 0U;
    for (uint8_t i = 0; i < NVM_SRC_PDO_MAX; ++i) {
        _status.pdoSrc[i].d32 = 0U;
        _srcPDO[i] = PDO();
    }
    return true;
}

bool STUSB4500::updatePDOSrc(void)
{
    waitUntilReady();

    //DELAY(TLOAD_REG_INIT_MS);

    static uint8_t const maxRequests = _srcCapRequestMax;
    uint8_t request = 0U;
    bool status = true;

    ++(_state.srcPDORequesting);

    while ((0U != _state.srcPDORequesting) && (request < maxRequests)) {
        if (!sendPDCableReset())
        { break; }
        processAlerts();
        ++request;
    }

    return status;
}

bool STUSB4500::updateRDOSnk(void)
{
    STUSB_GEN1S_RDO_REG_STATUS_RegTypeDef rdo;
    if (!wireRead(RDO_REG_STATUS, (uint8_t *)&rdo, sizeof(rdo)))
    { return false; }

    if (rdo.b.Object_Pos > 0) {

        _snkRDO.number        = rdo.b.Object_Pos;
        _snkRDO.current_mA    = rdo.b.OperatingCurrent * 10U;
        _snkRDO.maxCurrent_mA = rdo.b.MaxCurrent * 10U;

        if (_snkRDO.number <= _status.pdoSnkCount) {
            _snkRDO.voltage_mV =
                    _status.pdoSnk[_snkRDO.number - 1U].fix.Voltage * 50U;
        }
        return true;
    }
    else {
        _snkRDO = PDO();
        return false;
    }
}

/****
 * send a USB power delivery reset message, forcing the source to send all of
 * its available PDOs.
 ****/
bool STUSB4500::sendPDCableReset(void)
{
#define USBPD_HEADER_SOFT_RESET 0x0D
#define USBPD_PD_COMMAND        0x26

    CableStatus cable = cableStatus();
    if (!CABLE_CONNECTED(cable)) { return false; }

    // send PD message "soft reset" to source by setting TX header (0x51) to 0x0D,
    // and set PD command (0x1A) to 0x26.
    uint8_t data = USBPD_HEADER_SOFT_RESET;
    if (!wireWrite(TX_HEADER, &data, 1U))
    { return false; }

    uint8_t command = USBPD_PD_COMMAND;
    if (!wireWrite(STUSB_GEN1S_CMD_CTRL, &command, 1U))
    { return false; }

    return true;

#undef USBPD_HEADER_SOFT_RESET
#undef USBPD_PD_COMMAND
}

void STUSB4500::processAlerts(void)
{
#define READ_BUFF_MAX_SZ 40
    STUSB_GEN1S_ALERT_STATUS_RegTypeDef      alertStatus;
    STUSB_GEN1S_ALERT_STATUS_MASK_RegTypeDef alertMask;
    uint8_t buff[READ_BUFF_MAX_SZ];

    if (!wireRead(ALERT_STATUS_1, buff, 2U))
    { return; }

    alertMask.d8   = buff[1];
    alertStatus.d8 = buff[0] & ~(alertMask.d8);

    pushPDInterrupt(alertStatus.d8);

    if (0U != alertStatus.d8) {

        // bit 2
        if (0U != alertStatus.b.PRT_STATUS_AL) {

            USBPDMessageHeader header;

            if (!wireRead(PRT_STATUS, buff, 1U))
            { return; }
            _status.prtStatus.d8 = buff[0];

            if (1U == _status.prtStatus.b.MSG_RECEIVED) {

                if (!wireRead(RX_HEADER, buff, 2U))
                { return; }

                header.d16 = LE_u16(buff);

                if (header.b.dataObjectCount > 0) {

                    pushPDMessage(USBPD_MESSAGE_TYPE_DATA, header.b.messageType);

                    if (!wireRead(RX_BYTE_CNT, buff, 1U))
                    { return; }
                    uint8_t rxByteCount = buff[0];
                    if (rxByteCount != header.b.dataObjectCount * 4U)
                    { return; }

                    switch (header.b.messageType) {
                        case USBPD_DATAMSG_Source_Capabilities:

                            if (!wireRead(RX_DATA_OBJ, buff, rxByteCount))
                            { return; }

                            _status.pdoSrcCount = header.b.dataObjectCount;
                            for (uint8_t i = 0U, j = 0U;
                                 i < header.b.dataObjectCount;
                                 ++i, j += 4) {
                                _status.pdoSrc[i].d32 = LE_u32(&buff[j]);
                                if (0U == i)
                                { _status.pdoSrc[i].fix.Voltage = 100U; }
                                _srcPDO[i] = PDO(i + 1,
                                                 _status.pdoSrc[i].fix.Voltage * 50U,
                                                 _status.pdoSrc[i].fix.Max_Operating_Current * 10U);
                            }

                            _state.srcPDORequesting = 0U;
                            ++(_state.srcPDOReceived);
                            break;

                        case USBPD_DATAMSG_Request:
                        case USBPD_DATAMSG_Sink_Capabilities:
                        case USBPD_DATAMSG_Vendor_Defined:
                        default :
                            break;
                    }
                }
                else {

                    pushPDMessage(USBPD_MESSAGE_TYPE_CTRL, header.b.messageType);

                    switch (header.b.messageType) {
                        case USBPD_CTRLMSG_GoodCRC:
                            ++(_state.msgGoodCRC);
                            break;

                        case USBPD_CTRLMSG_Accept:
                            ++(_state.msgAccept);
                            break;

                        case USBPD_CTRLMSG_Reject:
                            ++(_state.msgReject);
                            break;

                        case USBPD_CTRLMSG_PS_RDY:
                            ++(_state.psrdyReceived);
                            break;

                        case USBPD_CTRLMSG_Reserved1:
                        case USBPD_CTRLMSG_Get_Source_Cap:
                        case USBPD_CTRLMSG_Get_Sink_Cap:
                        case USBPD_CTRLMSG_Wait:
                        case USBPD_CTRLMSG_Soft_Reset:
                        case USBPD_CTRLMSG_Not_Supported:
                        case USBPD_CTRLMSG_Get_Source_Cap_Extended:
                        case USBPD_CTRLMSG_Get_Status:
                        case USBPD_CTRLMSG_FR_Swap:
                        case USBPD_CTRLMSG_Get_PPS_Status:
                        case USBPD_CTRLMSG_Get_Country_Codes:
                        default:
                            break;
                    }
                }
            }
        }

        // bit 8
        _status.hwReset = buff[0] >> 7U;
        if (0U != _status.hwReset)
        { ++(_state.irqHardReset); }

        // bit 7
        if (0U != alertStatus.b.CC_DETECTION_STATUS_AL) {
            if (!wireRead(PORT_STATUS_TRANS, buff, 2U))
            { return; }
            _status.ccDetectionStatus.d8 = buff[1];
            if (0U != (buff[0] & STUSBMASK_ATTACH_STATUS_TRANS))
            { ++(_state.attachTransition); }
        }

        // bit 6
        if (0U != alertStatus.b.MONITORING_STATUS_AL) {
            if (!wireRead(TYPEC_MONITORING_STATUS_0, buff, 2U))
            { return; }
            _status.monitoringStatus.d8 = buff[1];
        }

        // always read & update CC attachement status
        if (!wireRead(CC_STATUS, buff, 1U))
        { return; }
        _status.ccStatus.d8 = buff[0];

        // bit 5
        if (0U != alertStatus.b.HW_FAULT_STATUS_AL) {
            if (!wireRead(CC_HW_FAULT_STATUS_0, buff, 2U))
            { return; }
            _status.hwFaultStatus.d8 = buff[1];
        }

    }

    return;
#undef READ_BUFF_MAX_SZ
}

void STUSB4500::processAttach(void)
{
    return;
}

bool STUSB4500::pushPDMessage(uint8_t const type, uint8_t const value)
{
    _state.msg[_state.msgHead] = type;
    ++(_state.msgHead);

    _state.msg[_state.msgHead] = value;
    ++(_state.msgHead);

    if (_state.msgHead == _state.msgTail)
    { return false; } // buffer overflow

    if (_state.msgHead >= USBPD_MESSAGE_QUEUE_SZ)
    { _state.msgHead = 0U; }

    ++(_state.msgReceived);

    return true;
}

bool STUSB4500::popPDMessage(uint8_t &type)
{
    if (_state.msgHead == _state.msgTail)
    { return false; } // empty buffer

    type = _state.msg[_state.msgTail];
    _state.msg[_state.msgTail] = 0U;

    ++(_state.msgTail);
    if (_state.msgTail >= USBPD_MESSAGE_QUEUE_SZ)
    { _state.msgTail = 0U; }

    return true;
}

bool STUSB4500::pushPDInterrupt(uint8_t const type)
{
    _state.irq[_state.irqHead] = type;
    ++(_state.irqHead);

    if (_state.irqHead == _state.irqTail)
    { return false; } // buffer overflow

    if (_state.irqHead >= USBPD_INTERRUPT_QUEUE_SZ)
    { _state.irqHead = 0U; }

    ++(_state.irqReceived);

    return true;
}

bool STUSB4500::popPDInterrupt(uint8_t &type)
{
    if (_state.irqHead == _state.irqTail)
    { return false; } // empty buffer

    type = _state.irq[_state.irqTail];
    _state.irq[_state.irqTail] = 0U;

    ++(_state.irqTail);
    if (_state.irqTail >= USBPD_INTERRUPT_QUEUE_SZ)
    { _state.irqTail = 0U; }

    return true;
}

#undef VERSION_MAJ
#undef VERSION_MIN
#undef VERSION_REV

#endif /* __STUSB4500_H__ */
