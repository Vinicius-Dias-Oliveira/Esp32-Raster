#include <LittleFS.h>
#include <ACAN2517.h>
#include <config.h>
#include <SPI.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <index.h>
#include <security.h>
#include <ArduinoJson.h>

static int MCP2517_SCK  = 12; 
static int MCP2517_MOSI = 11; 
static int MCP2517_MISO = 13; 
static int MCP2517_CS  = 10; 
static int MCP2517_INT = 21; 
static int LED_BUILTIN = 48; 

ACAN2517 can (MCP2517_CS, SPI, MCP2517_INT);
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// Define all CAN IDs to monitor
uint16_t canIDs[] = {0x280, 0x288, 0x380, 0x488, 0x588, 0x480, 0x320, 0x420, 0x470, 0x570, 0x5D0, 0x5D8, 0x10, 0x11, 0x520, 0x580};
const int numCANIDs = sizeof(canIDs) / sizeof(canIDs[0]);

// Structure to store CAN data with timestamp
struct CANFrameData {
  uint16_t id;
  uint8_t data[8];
  uint8_t length;
  unsigned long timestamp;
  bool hasNewData;
};

// Store recent data for all CAN IDs
CANFrameData canFrames[16];
uint16_t activeCANID = 0; // Currently selected CAN ID for detailed plotting


void sendAvailableCANIDs() {
  JsonDocument doc;
  doc["type"] = "availableCANIDs";
  JsonArray ids = doc.createNestedArray("canIDs");
  
  for (int i = 0; i < numCANIDs; i++) {
    ids.add("0x" + String(canIDs[i], HEX));
  }
  
  String jsonString;
  serializeJson(doc, jsonString);
  ws.textAll(jsonString);
}


void handleWebSocketMessage(String message) {
  JsonDocument doc;
  deserializeJson(doc, message);
  
  if (doc["action"] == "selectCANID") {
    String canIDStr = doc["canID"];
    canIDStr.replace("0x", "");
    activeCANID = strtol(canIDStr.c_str(), NULL, 16);
    Serial.print("Selected CAN ID: 0x");
    Serial.println(activeCANID, HEX);
    
    // Send confirmation back to client
    JsonDocument response;
    response["type"] = "canIDSelected";
    response["canID"] = "0x" + String(activeCANID, HEX);
    String responseStr;
    serializeJson(response, responseStr);
    ws.textAll(responseStr);
  }
}

void setup() {
  pinMode(AUX1, INPUT);
  pinMode(AUX2, INPUT);
  pinMode(AUX3, INPUT);
  pinMode(ENB, OUTPUT);
  pinMode(LED_CAN, OUTPUT);
  pinMode(LED_SD, OUTPUT);
  digitalWrite(ENB, HIGH);
  pinMode(LED_RGB, OUTPUT);
  digitalWrite(LED_RGB, LOW);
  
  Serial.begin(115200);
  while (!Serial) {
    delay(50);
    digitalWrite(LED_RGB, !digitalRead(LED_RGB));
  }

  // Initialize CAN frame data
  for (int i = 0; i < numCANIDs; i++) {
    canFrames[i].id = canIDs[i];
    canFrames[i].length = 0;
    canFrames[i].timestamp = 0;
    canFrames[i].hasNewData = false;
    memset(canFrames[i].data, 0, 8);
  }

  SPI.begin(MCP2517_SCK, MCP2517_MISO, MCP2517_MOSI);
  Serial.print("sizeof (ACAN2517Settings): ");
  Serial.print(sizeof(ACAN2517Settings));
  Serial.println(" bytes");
  Serial.println("Configure ACAN2517");
  
  ACAN2517Settings settings(ACAN2517Settings::OSC_40MHz, 500000);
  const uint32_t errorCode = can.begin(settings, [] { can.isr(); });
  
  if (errorCode == 0) {
    Serial.print("Bit Rate prescaler: ");
    Serial.println(settings.mBitRatePrescaler);
    Serial.print("Phase segment 1: ");
    Serial.println(settings.mPhaseSegment1);
    Serial.print("Phase segment 2: ");
    Serial.println(settings.mPhaseSegment2);
    Serial.print("SJW:");
    Serial.println(settings.mSJW);
    Serial.print("Actual bit rate: ");
    Serial.print(settings.actualBitRate());
    Serial.println(" bit/s");
    Serial.print("Exact bit rate ? ");
    Serial.println(settings.exactBitRate() ? "yes" : "no");
    Serial.print("Sample point: ");
    Serial.print(settings.samplePointFromBitStart());
    Serial.println("%");
  } else {
    Serial.print("Configuration error 0x");
    Serial.println(errorCode, HEX);
  }

  WiFi.mode(WIFI_MODE_AP);
  WiFi.softAP(ssid, password);
  Serial.print("WiFi AP started: ");
  Serial.println(WiFi.softAPIP());

  ws.onEvent([](AsyncWebSocket * server, AsyncWebSocketClient * client,
                AwsEventType type, void * arg, uint8_t *data, size_t len) {
    if (type == WS_EVT_CONNECT) {
      Serial.printf("WebSocket client #%u connected\n", client->id());
      sendAvailableCANIDs();
    } else if (type == WS_EVT_DISCONNECT) {
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
    } else if (type == WS_EVT_DATA) {
      // Handle incoming messages from client
      AwsFrameInfo * info = (AwsFrameInfo*)arg;
      if (info->final && info->index == 0 && info->len == len) {
        data[len] = 0;
        String message = (char*)data;
        handleWebSocketMessage(message);
      }
    }
  });
  
  server.addHandler(&ws);
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });
  
  if (!LittleFS.begin()) {
    Serial.println("Error mounting LittleFS");
    return;
  }
  
  server.serveStatic("/chart.js", LittleFS, "/chart.js");
  server.begin();
}

int findCANIndex(uint16_t id) {
  for (int i = 0; i < numCANIDs; i++) {
    if (canIDs[i] == id) {
      return i;
    }
  }
  return -1;
}

void sendByteData() {
  if (activeCANID == 0) return;
  
  int index = findCANIndex(activeCANID);
  if (index < 0 || !canFrames[index].hasNewData) return;
  
  JsonDocument doc;
  doc["type"] = "byteData";
  doc["canID"] = "0x" + String(activeCANID, HEX);
  doc["timestamp"] = millis();
  doc["frameLength"] = canFrames[index].length;
  
  JsonArray byteArray = doc.createNestedArray("bytes");
  for (int i = 0; i < 8; i++) {
    if (i < canFrames[index].length) {
      byteArray.add(canFrames[index].data[i]);
    } else {
      byteArray.add(0); // Pad with zeros for unused bytes
    }
  }
  
  String jsonString;
  serializeJson(doc, jsonString);
  ws.textAll(jsonString);
  
  canFrames[index].hasNewData = false;
}

void sendStatusUpdate() {
  JsonDocument doc;
  doc["type"] = "statusUpdate";
  doc["activeCANID"] = activeCANID > 0 ? "0x" + String(activeCANID, HEX) : "None";
  doc["timestamp"] = millis();
  
  // Send frame counts and last seen times for all CAN IDs
  JsonObject frameStats = doc.createNestedObject("frameStats");
  for (int i = 0; i < numCANIDs; i++) {
    String idStr = "0x" + String(canIDs[i], HEX);
    JsonObject stat = frameStats.createNestedObject(idStr);
    stat["lastSeen"] = canFrames[i].timestamp;
    stat["frameLength"] = canFrames[i].length;
  }
  
  String jsonString;
  serializeJson(doc, jsonString);
  ws.textAll(jsonString);
}

void loop() {
  CANMessage receivedFrame;

  if (can.available()) {
    can.receive(receivedFrame);
    
    int index = findCANIndex(receivedFrame.id);
    if (index >= 0) {
      // Store the received data
      canFrames[index].length = receivedFrame.len;
      canFrames[index].timestamp = millis();
      canFrames[index].hasNewData = true;
      
      for (int i = 0; i < receivedFrame.len && i < 8; i++) {
        canFrames[index].data[i] = receivedFrame.data[i];
      }
      
      digitalWrite(LED_CAN, HIGH);
    }

    // Debug output
    Serial.print("ID:0x");
    Serial.print(receivedFrame.id, HEX);
    Serial.print(" LEN:");
    Serial.print(receivedFrame.len);
    Serial.print(" DATA:");
    for (int i = 0; i < receivedFrame.len; i++) {
      Serial.print(receivedFrame.data[i]);
      Serial.print(" ");
    }
    Serial.println();
  }
  
  // Send byte data for selected CAN ID
  static unsigned long lastByteSend = 0;
  if (millis() - lastByteSend > 50) { // Send every 50ms for smooth plotting
    sendByteData();
    lastByteSend = millis();
  }
  
  // Send status updates less frequently
  static unsigned long lastStatusSend = 0;
  if (millis() - lastStatusSend > 500) { // Send every 1 second
    sendStatusUpdate();
    lastStatusSend = millis();
    digitalWrite(LED_CAN, LOW);
  }
  
  // Clean websocket clients
  ws.cleanupClients();
}