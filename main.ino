// USED LIBRARIES
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <sstream>


// WIFI CREDENTIALS
#define WIFI_SSID ""
#define WIFI_PASSWORD ""

// API CONFIG
#define API_METHOD "GET"
#define API_URL "http://127.0.0.1:3000/lights/getLights"

// OUTPUTS
#define RED_PIN D4
#define GREEN_PIN D7
#define BLUE_PIN D6

const int DELAY_MS = 2000;



void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  analogWriteRange(255);
  connectToWifi();
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
  String data = httpRequest(API_URL);
  Serial.print("Data: ");
  Serial.println(data);
  DynamicJsonDocument jsonDocument(2024);
  DeserializationError error = deserializeJson(jsonDocument, data);
  if (error) {
    Serial.println("There was an error while deserializing");
    Serial.print("Data: '");
    Serial.print(data);
    Serial.println("'");
    return;
  }
  JsonObject root = jsonDocument.as<JsonObject>();
  String color = root["Color"];

  int red = hexToInt(color.substring(1, 2));
  int green = hexToInt(color.substring(3, 2));
  int blue = hexToInt(color.substring(5, 2));
  digitalWrite(RED_PIN, red);
  digitalWrite(GREEN_PIN, green);
  digitalWrite(BLUE_PIN, blue);
  delay(DELAY_MS);
}

void connectToWifi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("");
  Serial.print("connecting to wifi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
}

String httpRequest(String url) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi Disconnected");
    return "";
  }
  HTTPClient http;
  http.useHTTP10(true);
  http.begin(url);
  int httpResponseCode = http.GET();
  String payload = "{}";
  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  http.end();
  return payload;
}

int hexToInt(String hex) {
    unsigned int bin;
    std::stringstream ss;
    ss << std::hex << hex;
    ss >> bin;
    return bin;
}