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


#include <ACAN2517.h>
#include <config.h>
#include <SPI.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <index.h>
#include <security.h>


static  int MCP2517_SCK  = 12 ; 
static  int MCP2517_MOSI = 11 ; 
static  int MCP2517_MISO = 13 ; 
static  int MCP2517_CS  = 10 ; 
static  int MCP2517_INT = 21 ; 
static  int LED_BUILTIN = 48 ; 

ACAN2517 can (MCP2517_CS, SPI, MCP2517_INT) ;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");


void setup () {
  pinMode(AUX1, INPUT);
  pinMode(AUX2, INPUT);
  pinMode(AUX3, INPUT);
  pinMode(ENB, OUTPUT);
  pinMode(LED_CAN, OUTPUT);
  pinMode(LED_SD, OUTPUT);
  digitalWrite(ENB, HIGH);
  pinMode(LED_RGB, OUTPUT);
  digitalWrite(LED_RGB, LOW);
  Serial.begin (115200);
  while (!Serial) {
    delay (50) ;
    digitalWrite (LED_RGB, !digitalRead (LED_RGB)) ;
  }

  SPI.begin (MCP2517_SCK, MCP2517_MISO, MCP2517_MOSI) ;
  Serial.print ("sizeof (ACAN2517Settings): ") ;
  Serial.print (sizeof (ACAN2517Settings)) ;
  Serial.println (" bytes") ;
  Serial.println ("Configure ACAN2517") ;
  ACAN2517Settings settings (ACAN2517Settings::OSC_40MHz, 500000) ;
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


  WiFi.mode(WIFI_MODE_AP);
  WiFi.softAP(ssid, password);
  Serial.print("WiFi AP started: ");
  Serial.println(WiFi.softAPIP());

  ws.onEvent([](AsyncWebSocket * server, AsyncWebSocketClient * client,
                AwsEventType type, void * arg, uint8_t *data, size_t len) {
  });
  server.addHandler(&ws);
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
  request->send_P(200, "text/html", index_html);
  });
  server.begin();

}


static uint32_t gBlinkLedDate = 0 ;
static uint32_t gReceivedFrameCount = 0 ;
static uint32_t gSentFrameCount = 0 ;

unsigned long rpm_id = 1416;
uint16_t rpm_value = 0;  // start from 0
uint16_t max_rpm = 6000;
uint8_t rpm_bytes[8] = {0};

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
  CANMessage receivedFrame;

  // rpm_value += 100; // increase 100 RPM per step
  // if (rpm_value > max_rpm) rpm_value = 0;

  // // Build the CAN message
  // frame.id = rpm_id;
  // frame.len = 8;
  // frame.ext = false;

  // // Set the data
  // frame.data[0] = (rpm_value >> 8) & 0xFF;  // MSB
  // frame.data[1] = 0x00;
  // frame.data[2] = rpm_value & 0xFF;         // LSB
  // frame.data[3] = 0x00;
  // frame.data[4] = 0x00;
  // frame.data[5] = 0x03;
  // frame.data[6] = 0x00;
  // frame.data[7] = 0x00;

  // // Send message
  // if (can.tryToSend(frame)) {
  //   Serial.print("Sent RPM: ");
  //   Serial.println(rpm_value);
  // } else {
  //   Serial.println("RPM Send Failed");
  // }

  // if (gBlinkLedDate < millis ()) {
  //   gBlinkLedDate += 100 ;
  //   digitalWrite (LED_RGB, !digitalRead (LED_RGB)) ;
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
    can.receive (receivedFrame) ;

    Serial.print("ID:0x");
    Serial.print(receivedFrame.id, HEX);
    // Serial.print("\",\"dlc\":");
    // Serial.print(receivedFrame.len);
    Serial.print(" DATA:");
    for (int i = 0; i < receivedFrame.len; i++) {
      Serial.print(receivedFrame.data[i]);
      Serial.print(" ");
      // if (i < receivedFrame.len - 1) Serial.print(",");
    }
    Serial.println("");
  }
  ws.cleanupClients();
}

