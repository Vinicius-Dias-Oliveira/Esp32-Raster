#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

/*********************************************************************************************************
 * USER CONFIG
 **********************************************************************************************************/
/*
* Board Version (This is now being defined in the PlatformIO ini file) 
*/
#define BOARDVERSION 5 // 2 = V4.0 / 1 = V3.30 / 0 = V3.20 or less 

/*
 * Program version (The first 3 digits should be in sync with the console version used for testing, the last two indicate minor release versions)
 */
#define CODEVERSION 4.2205

/*
 * Set the interface password
 */
#define PASSWORD F("SM4RT.W66DI7")

/*
 * Maximum time that the interface works after the user enters the password
 * Value in minutes
 */
#define PASSWORD_TIME 30

/*
*  Time After weedit power down
*/
#define POWER_DOWN_TIME 60
#define TIME_TO_WAKEUP 30   // Do not Used

/*********************************************************************************************************
 * DO NOT CHANGE ANYTHING AFTER HERE (Unlees you know what you're doing)
 **********************************************************************************************************/

#define SERIAL1 Serial1
#define WEEDIT Serial1
#define IDX_EEPROM_SAVE_MODE 100  // idx save machine mode
#define IDX_EEPROM_CAN_MAIN 52    // idx save CAN Main
#define IDX_EEPROM_POLLTIME   57  // idx pollTime value
#define IDX_EEPROM_CDDTIME    65  // idx CDD polltime
#define IDX_EEPROM_POWERSAVE  121  // idx Power Saving
#define IDX_NOZZLE_DELAY      45   // idx NOZZLE OFF DELAY
#define IDX_WDT_BAUD          40   // idx weedit serial baud

/*
 * BUFFER CONFIG
 */
#define BUFFER_SIZE 30           // Can buffer size
#define BUFFER_SIZE_WDT 50       // Wdt Rx buffer size
#define BUFFER_SIZE_NOVATEL 30   // Novatel buffer size
#define MSG_WDT_LENGTH  164      // Max Length message received from weedit
#define WDT_SENSORS 50           // Max number of sensors in the system
#define WDT_NOZZLES 200          // Max number of nozzles in the system

#define MAQ  0         // DO NOT CHANGE
#define INC  1         // DO NOT CHANGE
#define IMPLEMENT 0    // DO NOT CHANGE
#define TRACTOR 1      // DO NOT CHANGE
#define BOTH 2         // DO NOT CHANGE
#define DATALOGGER 91  // DO NOT CHANGE


/*
 * File Config
 */
#define FILE_SIZE_MAX 10  // in Mbytes
#define MSG_TO_FLUSH 10   //1000
#define TIME_AUTO_FLUSH 30  // Seconds

/*
 * Bluetooth Config
 */
#define TIME_MAX_BT_OFF 300 // Seconds
#define MAX_SEND_FREQ   100 // Hz

/*
 * Time that Weedit Flush is on
 */
#define OFF_DELAY_MS 2000 // Nozzle off delay Miliseconds

#define TIME_OUT_INC 60000 // Inc response timeout

#define LOG_SPEED_PULSES 1

#define AUX1 1
#define AUX2 2
#define AUX3 15 // NUC-BT

/*
* Wifi Interface TimeOut
*/
#define TIMEOUT_INTERFACE 120000



#if !(BOARDVERSION)
#define ENB 12
#define LED_CAN 2
#define LED_WDT 13
#define LED_RGB 48

#define CAN1_INT 15
#define CAN1_CS 21

#define CAN2_INT 15
#define CAN2_CS 21

#define SD_CS 22 // SD Chip Select
#define INC_RLY 32  // Reset NUC


#elif (BOARDVERSION == 3)

// I/O Configuration 
#define ENB 26  // Enable Peripherals
#define LED_CAN 27
#define LED_WDT 2

#define CAN1_INT 35 // CAN1 (IMPLEMENT)
#define CAN1_CS  25 // CAN1 (IMPLEMENT)

#define CAN2_INT 15 // CAN2 (TRACTOR)
#define CAN2_CS  21 // CAN2 (TRACTOR)

#define RX_AUX 33 // Serial(RS232) AUX
#define TX_AUX 32 // Serial(RS232) AUX
#define BAUD_SERIAL_AUX 230400 // Baud Serial

#define SD_CS 22 // SD Chip Select
#define INC_RLY 32  // Reset NUC

#elif(BOARDVERSION == 4)

// I/O Configuration 
#define ENB 14  // Enable Peripherals
#define LED_WDT 4
#define LED_SD 5
#define LED_CAN 6
#define LED_RGB 48

#define SD_CS 39 // SD Chip Select

#define INC_RLY 7  // Reset NUC
#define INC_BTN 15  // Enable NUC

#define CAN1_INT 21 // CAN1 (IMPLEMENT)
#define CAN1_CS  10 // CAN1 (IMPLEMENT)

#define CAN2_INT 47 // CAN2 (TRACTOR)
#define CAN2_CS  9 // CAN2 (TRACTOR)

#define RX_AUX 41 // Serial(RS232) AUX
#define TX_AUX 42 // Serial(RS232) AUX

#define AUX1 1
#define AUX2 2
#define AUX3 15 // NUC-BT

#elif(BOARDVERSION == 5) // V4.1

// I/O Configuration 
#define ENB 14  // Enable Peripherals
#define LED_WDT 4
#define LED_SD 5
#define LED_CAN 6
#define LED_RGB 48

#define SD_CS 39 // SD Chip Select

#define INC_RLY 7  // Reset NUC
#define INC_BTN 15  // Enable NUC

#define CAN1_INT 21 // CAN1 (IMPLEMENT)
#define CAN1_CS  10 // CAN1 (IMPLEMENT)

#define CAN2_INT 47 // CAN2 (TRACTOR)
#define CAN2_CS  9  // CAN2 (TRACTOR)

#define CAN3_CS 40  // CAN3 (AUX)
#define CAN3_INT 42 // CAN3 (AUX)

#define RX_AUX 41 // Serial(RS232) AUX
#define TX_AUX 42 // Serial(RS232) AUX

#define AUX1 1
#define AUX2 2  // ADC from imput voltage
#define AUX3 15 // NUC-BT

#elif(BOARDVERSION == 6) // V4.2

// I/O Configuration 
#define ENB 14  // Enable Peripherals
#define LED_WDT 4
#define LED_SD 5
#define LED_CAN 0
#define LED_RGB 48

#define SD_CS 39 // SD Chip Select

#define INC_RLY 7  // Reset NUC
#define INC_BTN 15  // Enable NUC

#define CAN1_INT 21 // CAN1 (IMPLEMENT)
#define CAN1_CS  10 // CAN1 (IMPLEMENT)

#define CAN2_INT 47 // CAN2 (TRACTOR)
#define CAN2_CS  9  // CAN2 (TRACTOR)

#define CAN3_CS 40  // CAN3 (AUX)
#define CAN3_INT 42 // CAN3 (AUX)

#define RX_AUX 41 // Serial(RS232) AUX
#define TX_AUX  6 // Serial(RS232) AUX

#define AUX1 1
#define AUX2 2  // ADC from imput voltage
#define AUX3 15 // NUC-BT

#endif

#endif