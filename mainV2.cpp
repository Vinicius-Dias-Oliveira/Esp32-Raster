#include <LittleFS.h>
#include <ACAN2517.h>
#include <config.h>
#include <SPI.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <index.h>
#include <security.h>
#include <LittleFS.h>


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
  if (!LittleFS.begin()) {
    Serial.println("Erro ao montar LittleFS");
    return;
  }
  server.serveStatic("/chart.js", LittleFS, "/chart.js");
  server.begin();

}

void loop () {
  CANMessage receivedFrame;

  if (can.available()) {
    can.receive(receivedFrame);

    uint16_t g1 = 0, g2 = 0, g3 = 0;
    bool shouldSend = false;

    if (receivedFrame.id == 0x280 && receivedFrame.len > 4) {
      g1 = receivedFrame.data[4];
      shouldSend = true;
    }

    if (receivedFrame.id == 0x488 && receivedFrame.len > 1) {
      g2 = receivedFrame.data[0];
      shouldSend = true;
    }

    if (receivedFrame.id == 0x488 && receivedFrame.len > 0) {
      g3 = receivedFrame.data[1];
      shouldSend = true;
    }

    if (shouldSend) {
      String json = "{\"g1\":" + String(g1) + 
                    ",\"g2\":" + String(g2) + 
                    ",\"g3\":" + String(g3) + "}";
      // Serial.println(json);
      ws.textAll(json);
    }

    Serial.print("ID:0x");
    Serial.print(receivedFrame.id, HEX);
    Serial.print(" DATA:");
    for (int i = 0; i < receivedFrame.len; i++) {
      Serial.print(receivedFrame.data[i]);
      Serial.print(" ");
    }
    Serial.println();
  }
}


// uint8_t g1 = 0, g2 = 0, g3 = 0;
// bool g1_updated = false, g2_updated = false, g3_updated = false;

// void loop() {
//   CANMessage receivedFrame;

//   if (can.available()) {
//     can.receive(receivedFrame);

//     if (receivedFrame.id == 0x280 && receivedFrame.len > 4) {
//       g1 = receivedFrame.data[4];
//       g1_updated = true;
//     }

//     if (receivedFrame.id == 0x488 && receivedFrame.len > 0) {
//       g2 = receivedFrame.data[0];
//       g2_updated = true;
//     }

//     if (receivedFrame.id == 0x488 && receivedFrame.len > 1) {
//       g3 = receivedFrame.data[1];
//       g3_updated = true;
//     }

//     if (g1_updated && g2_updated && g3_updated) {
//       String json = "{\"g1\":" + String(g1) + 
//                     ",\"g2\":" + String(g2) + 
//                     ",\"g3\":" + String(g3) + "}";

//       Serial.println(json);
//       ws.textAll(json);

//       // Reset flags
//       g1_updated = g2_updated = g3_updated = false;
//     }
//   }
// }