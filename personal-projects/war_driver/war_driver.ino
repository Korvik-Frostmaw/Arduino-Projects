include <WiFi.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include "secrets.h"

// Wi-Fi credentials used to connect this ESP32 to the local network.
const char* ssid = SECRET_SSID;
const char* pass = SECRET_PASS;

// Remote API endpoint that receives scan results.
const char* server = "";
int port = ;

StaticJsonDocument<4096> doc;

WiFiClient client;


void ScanWiFi() {

  int n = WiFi.scanNetworks();

  // Reuse the document to minimize heap fragmentation on embedded hardware.
  doc.clear();

  doc["device"] = "esp32-node-01";

  // Shared JSON document reused between scans to avoid repeated allocations.
  JsonArray networks = doc.createNestedArray("networks");

  if (n == 0) {
    Serial.println("No networks found");
    return;
  }

  for (int i = 0; i < n; i++) {

    JsonObject net = networks.createNestedObject();

    net["ssid"] = WiFi.SSID(i);
    net["rssi"] = WiFi.RSSI(i);
    net["ch"] = WiFi.channel(i);

    // Attempt to determine which encryption type is being used on the Access Point
    switch (WiFi.encryptionType(i)) {
      case WIFI_AUTH_OPEN: net["encryption"] = "open"; break;
      case WIFI_AUTH_WEP: net["encryption"] = "WEP"; break;
      case WIFI_AUTH_WPA_PSK: net["encryption"] = "WPA"; break;
      case WIFI_AUTH_WPA2_PSK: net["encryption"] = "WPA2"; break;
      case WIFI_AUTH_WPA3_PSK: net["encryption"] = "WPA3"; break;
      default: net["encryption"] = "unknown"; break;
    }
  }
}


void sendData() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected");
    return;
  }

  if (!client.connect(server, port)) {
    Serial.println("Connection failed");
    return;
  }

  // Serialize the JSON document into an HTTP request body.
  String body;
  serializeJson(doc, body);

  // Build packet headers
  client.print("POST /print_networks HTTP/1.1\r\n");
  client.print("Host: ");
  client.print(server);
  client.print("\r\n");
  client.print("Content-Type: application/json\r\n");
  client.print("Content-Length: ");
  client.print(body.length());
  client.print("\r\n");
  client.print("Connection: close\r\n\r\n");

  client.print(body);

  client.stop();

  Serial.println("Sent data");
}





void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, pass);

  Serial.println("Connecting to WiFi...");

  int attempts = 0;

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");

    attempts++;

    if (attempts > 20) {
      Serial.println("\nFAILED TO CONNECT");
      Serial.print("Status code: ");
      Serial.println(WiFi.status());
      break;
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected!");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Running without WiFi...");
  }
}



void loop() {

  ScanWiFi();
  sendData();

  // Scan every 10 seconds to reduce channel-hopping overhead and API traffic.
  delay(10000);
}
