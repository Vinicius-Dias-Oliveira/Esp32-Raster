// // #include <SPI.h>
// // #include <mcp2515.h>
// // #include <Arduino.h>

// // struct can_frame canMsg1;
// // MCP2515 mcp2515(5, 10000000, &SPI);


// // void setup() {
// //   canMsg1.can_id  = 0x0F6;
// //   canMsg1.can_dlc = 8;
// //   canMsg1.data[0] = 0x8E;
// //   canMsg1.data[1] = 0x87;
// //   canMsg1.data[2] = 0x32;
// //   canMsg1.data[3] = 0xFA;
// //   canMsg1.data[4] = 0x26;
// //   canMsg1.data[5] = 0x8E;
// //   canMsg1.data[6] = 0xBE;
// //   canMsg1.data[7] = 0x86;

  
// //   // while (!Serial);
// //   Serial.begin(115200);
// //   // SPI.begin(18, 19, 23, 5);
// //   SPI.begin(18, 19, 23, 21); //SPI PLACA BARROS 3.0

// //   Serial.println("Example: Write to CAN");
  
// //   mcp2515.reset();
// //   mcp2515.setBitrate(CAN_500KBPS);
// //   mcp2515.setNormalMode();
  
// //   Serial.println("Example: Write to CAN");
// // }

// // void loop() {
// //   mcp2515.sendMessage(&canMsg1);

// //   Serial.println("Messages sent");
  
// //   delay(1000);
// // }


// #include <SPI.h>
// #include <mcp2515.h>
// #include <Arduino.h>

// struct can_frame canMsg;
// // SPIClass mySPI(VSPI);

// MCP2515 mcp2515(21, 10000000, &SPI);

// void setup() {
//   Serial.begin(115200);
//   SPI.begin(18, 19, 23, 21);
  
//   mcp2515.reset();
//   mcp2515.setBitrate(CAN_500KBPS);
//   mcp2515.setNormalMode();
  
//   Serial.println("------- CAN Read ----------");
//   Serial.println("ID  DLC   DATA");
// }

// void loop() {
//   if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {
//     Serial.print(canMsg.can_id, HEX); // print ID
//     Serial.print(" "); 
//     Serial.print(canMsg.can_dlc, HEX); // print DLC
//     Serial.print(" ");
    
//     for (int i = 0; i<canMsg.can_dlc; i++)  {  // print the data
//       Serial.print(canMsg.data[i],HEX);
//       Serial.print(" ");
//     }

//     Serial.println();      
//   }
// }



// MISO 37
// MOSI 35
// SCK 36
// INTCAN1  21
// CS 10

// MCP2518FDT - E/SL 40mhz

#include <ACAN2517.h>
#include <config.h>
#include <SPI.h>

//——————————————————————————————————————————————————————————————————————————————
//  For using SPI on ESP32, see demo sketch SPI_Multiple_Buses
//  Two SPI busses are available in Arduino, HSPI and VSPI.
//  By default, Arduino SPI use VSPI, leaving HSPI unused.
//  Default VSPI pins are: SCK=18, MISO=19, MOSI=23.
//  You can change the default pin with additional begin arguments
//    SPI.begin (MCP2517_SCK, MCP2517_MISO, MCP2517_MOSI)
//  CS input of MCP2517 should be connected to a digital output port
//  INT output of MCP2517 should be connected to a digital input port, with interrupt capability
//  Notes:
//    - GPIOs 34 to 39 are GPIs – input only pins. These pins don’t have internal pull-ups or
//      pull-down resistors. They can’t be used as outputs.
//    - some pins do not support INPUT_PULLUP (see https://www.esp32.com/viewtopic.php?t=439)
//    - All GPIOs can be configured as interrupts
// See https://randomnerdtutorials.com/esp32-pinout-reference-gpios/
//——————————————————————————————————————————————————————————————————————————————

static  int MCP2517_SCK  = 12 ; // SCK input of MCP2517
static  int MCP2517_MOSI = 11 ; // SDI input of MCP2517
static  int MCP2517_MISO = 13 ; // SDO output of MCP2517

static  int MCP2517_CS  = 10 ; // CS input of MCP2517
static  int MCP2517_INT = 21 ; // INT output of MCP2517

static  int LED_BUILTIN = 48 ; // Builtin LED pin (GPIO2)

// MISO 37
// MOSI 35
// SCK 36
// INTCAN1  21
// CS 10

// MCP2518FDT - E/SL 40mhz

//——————————————————————————————————————————————————————————————————————————————
//  ACAN2517 Driver object
//——————————————————————————————————————————————————————————————————————————————

ACAN2517 can (MCP2517_CS, SPI, MCP2517_INT) ;


void setup () {
  pinMode(AUX1, INPUT);
  pinMode(AUX2, INPUT);
  pinMode(AUX3, INPUT);
  pinMode(ENB, OUTPUT);
  pinMode(LED_WDT, OUTPUT);
  pinMode(LED_CAN, OUTPUT);
  // pinMode(LED_SD, OUTPUT);
  digitalWrite(ENB, HIGH);
  pinMode(LED_RGB, OUTPUT);
  digitalWrite(LED_RGB, LOW);
//--- Switch on builtin led
  // pinMode (LED_BUILTIN, OUTPUT) ;
  // digitalWrite (LED_BUILTIN, HIGH) ;
//--- Start serial
  Serial.begin (115200);
//--- Wait for serial (blink led at 10 Hz during waiting)
  while (!Serial) {
    delay (50) ;
    digitalWrite (LED_BUILTIN, !digitalRead (LED_BUILTIN)) ;
  }
//----------------------------------- Begin SPI
  SPI.begin (MCP2517_SCK, MCP2517_MISO, MCP2517_MOSI) ;
//--- Configure ACAN2517
  Serial.print ("sizeof (ACAN2517Settings): ") ;
  Serial.print (sizeof (ACAN2517Settings)) ;
  Serial.println (" bytes") ;
  Serial.println ("Configure ACAN2517") ;
  // ACAN2517Settings settings (ACAN2517Settings::OSC_40MHz, 250000) ; // CAN bit rate 125 kb/s
  ACAN2517Settings settings (ACAN2517Settings::OSC_40MHz, 500000) ;
  // settings.mRequestedMode = ACAN2517Settings::InternalLoopBack ; // Select loopback mode
  const uint32_t errorCode = can.begin (settings, [] { can.isr () ; }) ;
  if (errorCode == 0) {
    Serial.print ("Bit Rate prescaler: ") ;
    Serial.println (settings.mBitRatePrescaler) ;
    Serial.print ("Phase segment 1: ") ;
    Serial.println (settings.mPhaseSegment1) ;
    Serial.print ("Phase segment 2: ") ;
    Serial.println (settings.mPhaseSegment2) ;
    Serial.print ("SJW:") ;
    Serial.println (settings.mSJW) ;
    Serial.print ("Actual bit rate: ") ;
    Serial.print (settings.actualBitRate ()) ;
    Serial.println (" bit/s") ;
    Serial.print ("Exact bit rate ? ") ;
    Serial.println (settings.exactBitRate () ? "yes" : "no") ;
    Serial.print ("Sample point: ") ;
    Serial.print (settings.samplePointFromBitStart ()) ;
    Serial.println ("%") ;
  }else{
    Serial.print ("Configuration error 0x") ;
    Serial.println (errorCode, HEX) ;
  }

  
  // digitalWrite(CAN1_INT, HIGH);

}

static uint32_t gBlinkLedDate = 0 ;
static uint32_t gReceivedFrameCount = 0 ;
static uint32_t gSentFrameCount = 0 ;

void loop () {
  struct CANMessage canMsg1;
  canMsg1.id  = 0x0F6;
  canMsg1.len = 8;
  canMsg1.data[0] = 0x8E;
  canMsg1.data[1] = 0x87;
  canMsg1.data[2] = 0x32;
  canMsg1.data[3] = 0xFA;
  canMsg1.data[4] = 0x26;
  canMsg1.data[5] = 0x8E;
  canMsg1.data[6] = 0xBE;
  canMsg1.data[7] = 0x86;

  CANMessage frame;

  // if (gBlinkLedDate < millis ()) {
  //   gBlinkLedDate += 2000 ;
  //   // digitalWrite (LED_BUILTIN, !digitalRead (LED_BUILTIN)) ;
  //   const bool ok = can.tryToSend (canMsg1) ;
  //   if (ok) {
  //     gSentFrameCount += 1 ;
  //     Serial.print ("Sent: ") ;
  //     Serial.println (gSentFrameCount) ;
  //   }else{
  //     Serial.println ("Send failure") ;
  //   }
  // }

  if (can.available ()) {
    can.receive (frame) ;
    gReceivedFrameCount ++ ;
    // Serial.print ("Received: ") ;
    // Serial.println (gReceivedFrameCount) ;
    Serial.print("ID: 0x");
    Serial.print(frame.id, HEX);
    
    // Print frame length
    Serial.print(", DLC: ");
    Serial.print(frame.len);
    
    // Print frame data bytes
    Serial.print(", Data: ");
    for(int i = 0; i < frame.len; i++) {
        Serial.print("0x");
        if(frame.data[i] < 16) Serial.print("0");  // Add leading zero for single digit hex
        Serial.print(frame.data[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
    
  } // Delay to avoid flooding the serial output
  // digitalWrite(CAN1_INT, LOW);
}

